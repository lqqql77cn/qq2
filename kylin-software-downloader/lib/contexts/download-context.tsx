import React, { createContext, useContext, useReducer, useEffect } from "react";
import AsyncStorage from "@react-native-async-storage/async-storage";

// Types
export interface DownloadSource {
  id: string;
  name: string;
  url: string;
  type: "apt" | "yum" | "dnf" | "custom";
  username?: string;
  password?: string;
  enabled: boolean;
  priority: number;
}

export interface SoftwarePackage {
  id: string;
  name: string;
  version: string;
  size: number; // in bytes
  description: string;
  dependencies: string[]; // package names
  sourceId: string;
}

export interface DownloadSettings {
  targetSystem: "kylin" | "other";
  targetSystemVersion: string;
  targetArchitecture: "arm64" | "armv7" | "x86_64" | "x86";
  saveDirectory: string;
  analyzeDependencies: boolean;
  autoPackAfterDownload: boolean;
  packageFormat: "tar.gz" | "zip";
}

export interface DownloadState {
  sources: DownloadSource[];
  selectedPackages: string[]; // package IDs
  settings: DownloadSettings;
  downloadHistory: Array<{
    id: string;
    timestamp: number;
    packageNames: string[];
    outputPath: string;
  }>;
}

export type DownloadAction =
  | { type: "ADD_SOURCE"; payload: DownloadSource }
  | { type: "UPDATE_SOURCE"; payload: DownloadSource }
  | { type: "DELETE_SOURCE"; payload: string }
  | { type: "REORDER_SOURCES"; payload: DownloadSource[] }
  | { type: "SELECT_PACKAGE"; payload: string }
  | { type: "DESELECT_PACKAGE"; payload: string }
  | { type: "SELECT_ALL_PACKAGES"; payload: string[] }
  | { type: "DESELECT_ALL_PACKAGES" }
  | { type: "UPDATE_SETTINGS"; payload: Partial<DownloadSettings> }
  | { type: "ADD_DOWNLOAD_HISTORY"; payload: DownloadState["downloadHistory"][0] }
  | { type: "LOAD_STATE"; payload: DownloadState };

// Initial state
const initialState: DownloadState = {
  sources: [],
  selectedPackages: [],
  settings: {
    targetSystem: "kylin",
    targetSystemVersion: "10",
    targetArchitecture: "arm64",
    saveDirectory: "",
    analyzeDependencies: true,
    autoPackAfterDownload: true,
    packageFormat: "tar.gz",
  },
  downloadHistory: [],
};

// Reducer
function downloadReducer(state: DownloadState, action: DownloadAction): DownloadState {
  switch (action.type) {
    case "ADD_SOURCE":
      return {
        ...state,
        sources: [...state.sources, action.payload],
      };

    case "UPDATE_SOURCE":
      return {
        ...state,
        sources: state.sources.map((s) => (s.id === action.payload.id ? action.payload : s)),
      };

    case "DELETE_SOURCE":
      return {
        ...state,
        sources: state.sources.filter((s) => s.id !== action.payload),
      };

    case "REORDER_SOURCES":
      return {
        ...state,
        sources: action.payload,
      };

    case "SELECT_PACKAGE":
      return {
        ...state,
        selectedPackages: [...new Set([...state.selectedPackages, action.payload])],
      };

    case "DESELECT_PACKAGE":
      return {
        ...state,
        selectedPackages: state.selectedPackages.filter((p) => p !== action.payload),
      };

    case "SELECT_ALL_PACKAGES":
      return {
        ...state,
        selectedPackages: action.payload,
      };

    case "DESELECT_ALL_PACKAGES":
      return {
        ...state,
        selectedPackages: [],
      };

    case "UPDATE_SETTINGS":
      return {
        ...state,
        settings: { ...state.settings, ...action.payload },
      };

    case "ADD_DOWNLOAD_HISTORY":
      return {
        ...state,
        downloadHistory: [action.payload, ...state.downloadHistory].slice(0, 50), // Keep last 50
      };

    case "LOAD_STATE":
      return action.payload;

    default:
      return state;
  }
}

// Context
interface DownloadContextType {
  state: DownloadState;
  dispatch: React.Dispatch<DownloadAction>;
  addSource: (source: Omit<DownloadSource, "id">) => void;
  updateSource: (id: string, updates: Partial<DownloadSource>) => void;
  deleteSource: (id: string) => void;
  selectPackage: (packageId: string) => void;
  deselectPackage: (packageId: string) => void;
  selectAllPackages: (packageIds: string[]) => void;
  deselectAllPackages: () => void;
  updateSettings: (settings: Partial<DownloadSettings>) => void;
  addDownloadHistory: (history: DownloadState["downloadHistory"][0]) => void;
}

const DownloadContext = createContext<DownloadContextType | undefined>(undefined);

// Provider
export function DownloadProvider({ children }: { children: React.ReactNode }) {
  const [state, dispatch] = useReducer(downloadReducer, initialState);

  // Load state from AsyncStorage on mount
  useEffect(() => {
    loadStateFromStorage();
  }, []);

  // Save state to AsyncStorage whenever it changes
  useEffect(() => {
    saveStateToStorage(state);
  }, [state]);

  const loadStateFromStorage = async () => {
    try {
      const savedState = await AsyncStorage.getItem("downloadState");
      if (savedState) {
        dispatch({ type: "LOAD_STATE", payload: JSON.parse(savedState) });
      }
    } catch (error) {
      console.error("Failed to load state from storage:", error);
    }
  };

  const saveStateToStorage = async (stateToSave: DownloadState) => {
    try {
      await AsyncStorage.setItem("downloadState", JSON.stringify(stateToSave));
    } catch (error) {
      console.error("Failed to save state to storage:", error);
    }
  };

  const addSource = (source: Omit<DownloadSource, "id">) => {
    const id = `source_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
    dispatch({
      type: "ADD_SOURCE",
      payload: { ...source, id },
    });
  };

  const updateSource = (id: string, updates: Partial<DownloadSource>) => {
    const source = state.sources.find((s) => s.id === id);
    if (source) {
      dispatch({
        type: "UPDATE_SOURCE",
        payload: { ...source, ...updates },
      });
    }
  };

  const deleteSource = (id: string) => {
    dispatch({ type: "DELETE_SOURCE", payload: id });
  };

  const selectPackage = (packageId: string) => {
    dispatch({ type: "SELECT_PACKAGE", payload: packageId });
  };

  const deselectPackage = (packageId: string) => {
    dispatch({ type: "DESELECT_PACKAGE", payload: packageId });
  };

  const selectAllPackages = (packageIds: string[]) => {
    dispatch({ type: "SELECT_ALL_PACKAGES", payload: packageIds });
  };

  const deselectAllPackages = () => {
    dispatch({ type: "DESELECT_ALL_PACKAGES" });
  };

  const updateSettings = (settings: Partial<DownloadSettings>) => {
    dispatch({ type: "UPDATE_SETTINGS", payload: settings });
  };

  const addDownloadHistory = (history: DownloadState["downloadHistory"][0]) => {
    dispatch({ type: "ADD_DOWNLOAD_HISTORY", payload: history });
  };

  const value: DownloadContextType = {
    state,
    dispatch,
    addSource,
    updateSource,
    deleteSource,
    selectPackage,
    deselectPackage,
    selectAllPackages,
    deselectAllPackages,
    updateSettings,
    addDownloadHistory,
  };

  return <DownloadContext.Provider value={value}>{children}</DownloadContext.Provider>;
}

// Hook
export function useDownload() {
  const context = useContext(DownloadContext);
  if (!context) {
    throw new Error("useDownload must be used within DownloadProvider");
  }
  return context;
}
