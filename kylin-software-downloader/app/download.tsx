import { ScrollView, Text, View, TouchableOpacity, Pressable, FlatList } from "react-native";
import { useRouter } from "expo-router";
import { ScreenContainer } from "@/components/screen-container";
import { useDownload } from "@/lib/contexts/download-context";
import { useColors } from "@/hooks/use-colors";
import { MaterialIcons } from "@expo/vector-icons";
import { useState } from "react";

interface LogEntry {
  id: string;
  timestamp: number;
  level: "info" | "success" | "warning" | "error";
  message: string;
}

export default function DownloadScreen() {
  const router = useRouter();
  const colors = useColors();
  const { state, addDownloadHistory } = useDownload();
  const [downloading, setDownloading] = useState(false);
  const [progress, setProgress] = useState(0);
  const [logs, setLogs] = useState<LogEntry[]>([]);
  const [completed, setCompleted] = useState(false);
  const [outputPath, setOutputPath] = useState("");

  const addLog = (message: string, level: "info" | "success" | "warning" | "error" = "info") => {
    const newLog: LogEntry = {
      id: `log_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`,
      timestamp: Date.now(),
      level,
      message,
    };
    setLogs((prev) => [...prev, newLog]);
  };

  const formatSize = (bytes: number) => {
    if (bytes === 0) return "0 B";
    const k = 1024;
    const sizes = ["B", "KB", "MB", "GB"];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return Math.round((bytes / Math.pow(k, i)) * 100) / 100 + " " + sizes[i];
  };

  const handleStartDownload = async () => {
    if (state.selectedPackages.length === 0) {
      addLog("错误: 请先选择要下载的软件包", "error");
      return;
    }

    if (!state.settings.saveDirectory) {
      addLog("错误: 请先设置保存目录", "error");
      return;
    }

    setDownloading(true);
    setProgress(0);
    setLogs([]);
    setCompleted(false);
    setOutputPath("");

    try {
      addLog("开始下载...", "info");
      addLog(`目标系统: ${state.settings.targetSystem} (${state.settings.targetArchitecture})`, "info");
      addLog(`已选择 ${state.selectedPackages.length} 个软件包`, "info");

      // Simulate download process
      for (let i = 0; i <= 100; i += 10) {
        await new Promise((resolve) => setTimeout(resolve, 500));
        setProgress(i);
        if (i === 30) {
          addLog("正在分析依赖关系...", "info");
        } else if (i === 60) {
          addLog("依赖分析完成，开始下载", "success");
        } else if (i === 90) {
          addLog("下载完成，开始打包...", "info");
        }
      }

      setProgress(100);
      const timestamp = new Date().toISOString().replace(/[:.]/g, "-");
      const path = `${state.settings.saveDirectory}/kylin-packages-${timestamp}.${state.settings.packageFormat === "tar.gz" ? "tar.gz" : "zip"}`;
      setOutputPath(path);

      addLog("打包完成！", "success");
      addLog(`输出路径: ${path}`, "success");
      setCompleted(true);

      // Add to history
      addDownloadHistory({
        id: `download_${Date.now()}`,
        timestamp: Date.now(),
        packageNames: state.selectedPackages.slice(0, 3).map((id) => id),
        outputPath: path,
      });
    } catch (error) {
      addLog(`下载失败: ${error instanceof Error ? error.message : "未知错误"}`, "error");
    } finally {
      setDownloading(false);
    }
  };

  const getTotalSize = () => {
    // Mock calculation
    return state.selectedPackages.length * 1024 * 1024 * 5; // 5MB per package
  };

  const renderLogEntry = ({ item }: { item: LogEntry }) => {
    const levelColors = {
      info: { bg: "bg-blue-50", text: "text-blue-700", icon: "info" },
      success: { bg: "bg-green-50", text: "text-green-700", icon: "check-circle" },
      warning: { bg: "bg-yellow-50", text: "text-yellow-700", icon: "warning" },
      error: { bg: "bg-red-50", text: "text-red-700", icon: "error" },
    };

    const color = levelColors[item.level];
    const iconColor = item.level === "success" ? "#22C55E" : item.level === "error" ? "#EF4444" : "#0066CC";

    return (
      <View className={`flex-row gap-2 p-3 rounded-lg mb-2 ${color.bg}`}>
        <MaterialIcons name={color.icon as any} size={16} color={iconColor} />
        <Text className={`flex-1 text-xs ${color.text}`}>{item.message}</Text>
      </View>
    );
  };

  return (
    <ScreenContainer className="p-4">
      <ScrollView contentContainerStyle={{ flexGrow: 1 }} showsVerticalScrollIndicator={false}>
        <View className="gap-4">
          {/* Header */}
          <View className="gap-2 mb-2">
            <Text className="text-2xl font-bold text-foreground">打包与下载</Text>
            <Text className="text-sm text-muted">下载软件包并打包为安装包</Text>
          </View>

          {/* Configuration Summary */}
          {!downloading && !completed && (
            <View className="bg-surface rounded-lg border border-border p-4 gap-3">
              <Text className="text-base font-semibold text-foreground">配置摘要</Text>
              <View className="gap-2">
                <View className="flex-row items-center justify-between">
                  <Text className="text-sm text-muted">已选软件:</Text>
                  <Text className="text-sm font-semibold text-foreground">{state.selectedPackages.length} 个</Text>
                </View>
                <View className="flex-row items-center justify-between">
                  <Text className="text-sm text-muted">预计大小:</Text>
                  <Text className="text-sm font-semibold text-foreground">{formatSize(getTotalSize())}</Text>
                </View>
                <View className="flex-row items-center justify-between">
                  <Text className="text-sm text-muted">目标系统:</Text>
                  <Text className="text-sm font-semibold text-foreground">
                    {state.settings.targetSystem} ({state.settings.targetArchitecture})
                  </Text>
                </View>
                <View className="flex-row items-center justify-between">
                  <Text className="text-sm text-muted">打包格式:</Text>
                  <Text className="text-sm font-semibold text-foreground">{state.settings.packageFormat}</Text>
                </View>
              </View>
            </View>
          )}

          {/* Progress */}
          {(downloading || completed) && (
            <View className="bg-surface rounded-lg border border-border p-4 gap-3">
              <View className="flex-row items-center justify-between mb-2">
                <Text className="text-base font-semibold text-foreground">进度</Text>
                <Text className="text-base font-bold text-primary">{progress}%</Text>
              </View>
              <View className="h-2 bg-border rounded-full overflow-hidden">
                <View
                  className="h-full bg-primary"
                  style={{ width: `${progress}%` }}
                />
              </View>
            </View>
          )}

          {/* Logs */}
          <View className="bg-surface rounded-lg border border-border p-4 gap-2">
            <Text className="text-base font-semibold text-foreground mb-2">日志输出</Text>
            {logs.length === 0 ? (
              <Text className="text-sm text-muted text-center py-4">点击开始下载查看日志</Text>
            ) : (
              <FlatList
                data={logs}
                renderItem={renderLogEntry}
                keyExtractor={(item) => item.id}
                scrollEnabled={false}
              />
            )}
          </View>

          {/* Completed State */}
          {completed && outputPath && (
            <View className="bg-success bg-opacity-10 rounded-lg border border-success border-opacity-20 p-4 gap-3">
              <View className="flex-row gap-2 items-start">
                <MaterialIcons name="check-circle" size={20} color="#22C55E" />
                <Text className="flex-1 text-sm font-semibold text-success">下载和打包完成！</Text>
              </View>
              <View className="bg-white bg-opacity-50 rounded-lg p-3 gap-1">
                <Text className="text-xs text-muted">输出路径:</Text>
                <Text className="text-sm text-foreground break-words">{outputPath}</Text>
              </View>
              <TouchableOpacity
                className="bg-success rounded-lg py-2 px-4 items-center mt-2"
                onPress={() => {
                  setCompleted(false);
                  setLogs([]);
                  setProgress(0);
                }}
              >
                <Text className="text-white font-semibold text-sm">开始新的下载</Text>
              </TouchableOpacity>
            </View>
          )}

          {/* Actions */}
          <View className="flex-row gap-3">
            <TouchableOpacity
              className="flex-1 bg-surface border border-border rounded-lg py-3 items-center"
              onPress={() => router.back()}
              disabled={downloading}
            >
              <Text className="text-sm font-semibold text-foreground">返回</Text>
            </TouchableOpacity>
            {!completed && (
              <TouchableOpacity
                className="flex-1 bg-primary rounded-lg py-3 items-center"
                onPress={handleStartDownload}
                disabled={downloading || state.selectedPackages.length === 0}
              >
                <Text className="text-sm font-semibold text-white">
                  {downloading ? "下载中..." : "开始下载"}
                </Text>
              </TouchableOpacity>
            )}
          </View>
        </View>
      </ScrollView>
    </ScreenContainer>
  );
}
