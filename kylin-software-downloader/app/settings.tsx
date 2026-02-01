import { ScrollView, Text, View, TouchableOpacity, Pressable, Alert } from "react-native";
import { useRouter } from "expo-router";
import { ScreenContainer } from "@/components/screen-container";
import { useDownload } from "@/lib/contexts/download-context";
import { useColors } from "@/hooks/use-colors";
import { MaterialIcons } from "@expo/vector-icons";
import { useState } from "react";
import { Picker } from "@react-native-picker/picker";
import * as DocumentPicker from "expo-document-picker";
import type { DocumentPickerAsset } from "expo-document-picker";

export default function SettingsScreen() {
  const router = useRouter();
  const colors = useColors();
  const { state, updateSettings } = useDownload();
  const [selectedDir, setSelectedDir] = useState(state.settings.saveDirectory);

  const handleSelectDirectory = async () => {
    try {
      const result = await DocumentPicker.getDocumentAsync({
        type: "*/*",
        copyToCacheDirectory: false,
      }) as any;
      if (!result.cancelled && (result as any).uri) {
        const uri = (result as any).uri;
        setSelectedDir(uri);
        updateSettings({ saveDirectory: uri });
      }
    } catch (error) {
      Alert.alert("错误", "选择目录失败");
    }
  };

  return (
    <ScreenContainer className="p-4">
      <ScrollView contentContainerStyle={{ flexGrow: 1 }} showsVerticalScrollIndicator={false}>
        <View className="gap-4">
          {/* Header */}
          <View className="gap-2 mb-2">
            <Text className="text-2xl font-bold text-foreground">下载设置</Text>
            <Text className="text-sm text-muted">配置下载和打包参数</Text>
          </View>

          {/* Target System Section */}
          <View className="gap-3">
            <Text className="text-base font-semibold text-foreground">目标系统</Text>
            <View className="bg-surface border border-border rounded-lg overflow-hidden">
              <Picker
                selectedValue={state.settings.targetSystem}
                onValueChange={(value: any) => updateSettings({ targetSystem: value })}
                style={{ color: colors.foreground }}
              >
                <Picker.Item label="银河麒麟" value="kylin" />
                <Picker.Item label="其他 Linux" value="other" />
              </Picker>
            </View>
          </View>

          {/* Target Architecture Section */}
          <View className="gap-3">
            <Text className="text-base font-semibold text-foreground">目标架构</Text>
            <View className="gap-2">
              {[
                { label: "ARM 64-bit (arm64)", value: "arm64" },
                { label: "ARM 32-bit (armv7)", value: "armv7" },
                { label: "x86 64-bit (x86_64)", value: "x86_64" },
                { label: "x86 32-bit (x86)", value: "x86" },
              ].map((arch) => (
                <Pressable
                  key={arch.value}
                  onPress={() => updateSettings({ targetArchitecture: arch.value as any })}
                  style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }]}
                >
                  <View className="flex-row items-center gap-3 bg-surface rounded-lg p-4 border border-border">
                    <View
                      className={`w-5 h-5 rounded-full border-2 items-center justify-center ${
                        state.settings.targetArchitecture === arch.value
                          ? "bg-primary border-primary"
                          : "border-border"
                      }`}
                    >
                      {state.settings.targetArchitecture === arch.value && (
                        <View className="w-2 h-2 bg-white rounded-full" />
                      )}
                    </View>
                    <View className="flex-1">
                      <Text className="text-sm font-medium text-foreground">{arch.label}</Text>
                      {arch.value === "arm64" && (
                        <Text className="text-xs text-muted mt-1">推荐用于银河麒麟系统</Text>
                      )}
                    </View>
                  </View>
                </Pressable>
              ))}
            </View>
          </View>

          {/* Save Directory Section */}
          <View className="gap-3">
            <Text className="text-base font-semibold text-foreground">保存目录</Text>
            <TouchableOpacity
              className="bg-surface border border-border rounded-lg p-4 flex-row items-center justify-between"
              onPress={handleSelectDirectory}
            >
              <View className="flex-1">
                <Text className="text-sm font-medium text-foreground mb-1">选择保存位置</Text>
                <Text className="text-xs text-muted" numberOfLines={1}>
                  {selectedDir || "未选择"}
                </Text>
              </View>
              <MaterialIcons name="folder-open" size={20} color={colors.primary} />
            </TouchableOpacity>
          </View>

          {/* Download Options Section */}
          <View className="gap-3">
            <Text className="text-base font-semibold text-foreground">下载选项</Text>

            {/* Analyze Dependencies */}
            <View className="flex-row items-center justify-between bg-surface rounded-lg p-4 border border-border">
              <View className="flex-1">
                <Text className="text-sm font-medium text-foreground">自动分析依赖</Text>
                <Text className="text-xs text-muted mt-1">下载时自动获取依赖包</Text>
              </View>
              <Pressable
                onPress={() =>
                  updateSettings({
                    analyzeDependencies: !state.settings.analyzeDependencies,
                  })
                }
                style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }]}
              >
                <View
                  className={`w-12 h-6 rounded-full items-center justify-center ${
                    state.settings.analyzeDependencies ? "bg-success" : "bg-muted"
                  }`}
                >
                  <View
                    className="w-5 h-5 rounded-full bg-white"
                    style={{
                      transform: [{ translateX: state.settings.analyzeDependencies ? 8 : -8 }],
                    }}
                  />
                </View>
              </Pressable>
            </View>

            {/* Auto Pack After Download */}
            <View className="flex-row items-center justify-between bg-surface rounded-lg p-4 border border-border">
              <View className="flex-1">
                <Text className="text-sm font-medium text-foreground">下载后自动打包</Text>
                <Text className="text-xs text-muted mt-1">完成下载后自动打包为安装包</Text>
              </View>
              <Pressable
                onPress={() =>
                  updateSettings({
                    autoPackAfterDownload: !state.settings.autoPackAfterDownload,
                  })
                }
                style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }]}
              >
                <View
                  className={`w-12 h-6 rounded-full items-center justify-center ${
                    state.settings.autoPackAfterDownload ? "bg-success" : "bg-muted"
                  }`}
                >
                  <View
                    className="w-5 h-5 rounded-full bg-white"
                    style={{
                      transform: [{ translateX: state.settings.autoPackAfterDownload ? 8 : -8 }],
                    }}
                  />
                </View>
              </Pressable>
            </View>

            {/* Package Format */}
            <View className="gap-2">
              <Text className="text-sm font-medium text-foreground">打包格式</Text>
              <View className="bg-surface border border-border rounded-lg overflow-hidden">
                <Picker
                  selectedValue={state.settings.packageFormat}
                  onValueChange={(value: any) => updateSettings({ packageFormat: value })}
                  style={{ color: colors.foreground }}
                >
                  <Picker.Item label="TAR.GZ (推荐)" value="tar.gz" />
                  <Picker.Item label="ZIP" value="zip" />
                </Picker>
              </View>
            </View>
          </View>

          {/* Info Box */}
          <View className="bg-primary bg-opacity-10 rounded-lg p-4 border border-primary border-opacity-20 gap-2">
            <View className="flex-row gap-2 items-start">
              <MaterialIcons name="info" size={20} color="#0066CC" />
              <Text className="flex-1 text-sm text-foreground leading-relaxed">
                这些设置将应用于所有后续的下载操作。建议保持默认设置以获得最佳体验。
              </Text>
            </View>
          </View>
        </View>
      </ScrollView>
    </ScreenContainer>
  );
}
