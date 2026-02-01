import { ScrollView, Text, View, TextInput, TouchableOpacity, Pressable, Alert } from "react-native";
import { useRouter, useLocalSearchParams } from "expo-router";
import { ScreenContainer } from "@/components/screen-container";
import { useDownload, DownloadSource } from "@/lib/contexts/download-context";
import { useColors } from "@/hooks/use-colors";
import { MaterialIcons } from "@expo/vector-icons";
import { useState, useEffect } from "react";
import { Picker } from "@react-native-picker/picker";

export default function SourceFormScreen() {
  const router = useRouter();
  const colors = useColors();
  const { sourceId } = useLocalSearchParams<{ sourceId?: string }>();
  const { state, addSource, updateSource } = useDownload();

  const [name, setName] = useState("");
  const [url, setUrl] = useState("");
  const [type, setType] = useState<"apt" | "yum" | "dnf" | "custom">("apt");
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [enabled, setEnabled] = useState(true);
  const [testing, setTesting] = useState(false);
  const [testResult, setTestResult] = useState<{ success: boolean; message: string } | null>(null);

  useEffect(() => {
    if (sourceId) {
      const source = state.sources.find((s) => s.id === sourceId);
      if (source) {
        setName(source.name);
        setUrl(source.url);
        setType(source.type);
        setUsername(source.username || "");
        setPassword(source.password || "");
        setEnabled(source.enabled);
      }
    }
  }, [sourceId, state.sources]);

  const handleTestConnection = async () => {
    if (!url) {
      Alert.alert("错误", "请输入下载源 URL");
      return;
    }

    setTesting(true);
    setTestResult(null);

    try {
      // Simulate connection test
      const response = await fetch(url, { method: "HEAD" });
      if (response.ok || response.status === 405) {
        // 405 Method Not Allowed is acceptable for HEAD request
        setTestResult({ success: true, message: "连接成功" });
      } else {
        setTestResult({ success: false, message: `连接失败: ${response.status}` });
      }
    } catch (error) {
      setTestResult({ success: false, message: `连接失败: ${error instanceof Error ? error.message : "未知错误"}` });
    } finally {
      setTesting(false);
    }
  };

  const handleSave = () => {
    if (!name.trim()) {
      Alert.alert("错误", "请输入下载源名称");
      return;
    }
    if (!url.trim()) {
      Alert.alert("错误", "请输入下载源 URL");
      return;
    }

    if (sourceId) {
      updateSource(sourceId, {
        name,
        url,
        type,
        username: username || undefined,
        password: password || undefined,
        enabled,
      });
    } else {
      addSource({
        name,
        url,
        type,
        username: username || undefined,
        password: password || undefined,
        enabled,
        priority: state.sources.length,
      });
    }

    router.back();
  };

  return (
    <ScreenContainer className="p-4">
      <ScrollView contentContainerStyle={{ flexGrow: 1 }} showsVerticalScrollIndicator={false}>
        <View className="gap-4">
          {/* Header */}
          <View className="flex-row items-center justify-between mb-2">
            <View>
              <Text className="text-2xl font-bold text-foreground">
                {sourceId ? "编辑下载源" : "添加下载源"}
              </Text>
            </View>
            <Pressable
              onPress={() => router.back()}
              style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }]}
            >
              <MaterialIcons name="close" size={24} color={colors.foreground} />
            </Pressable>
          </View>

          {/* Form */}
          <View className="gap-4">
            {/* Source Name */}
            <View className="gap-2">
              <Text className="text-sm font-semibold text-foreground">源名称 *</Text>
              <TextInput
                value={name}
                onChangeText={setName}
                placeholder="例如: 官方源、阿里源"
                placeholderTextColor={colors.muted}
                className="bg-surface border border-border rounded-lg px-4 py-3 text-foreground"
              />
            </View>

            {/* Source URL */}
            <View className="gap-2">
              <Text className="text-sm font-semibold text-foreground">源 URL *</Text>
              <TextInput
                value={url}
                onChangeText={setUrl}
                placeholder="例如: https://repo.example.com/kylin"
                placeholderTextColor={colors.muted}
                className="bg-surface border border-border rounded-lg px-4 py-3 text-foreground"
              />
            </View>

            {/* Source Type */}
            <View className="gap-2">
              <Text className="text-sm font-semibold text-foreground">源类型</Text>
              <View className="bg-surface border border-border rounded-lg overflow-hidden">
                <Picker
                  selectedValue={type}
                  onValueChange={(value: any) => setType(value as any)}
                  style={{ color: colors.foreground }}
                >
                  <Picker.Item label="APT (Debian/Ubuntu)" value="apt" />
                  <Picker.Item label="YUM (RedHat/CentOS)" value="yum" />
                  <Picker.Item label="DNF (Fedora)" value="dnf" />
                  <Picker.Item label="自定义" value="custom" />
                </Picker>
              </View>
            </View>

            {/* Username */}
            <View className="gap-2">
              <Text className="text-sm font-semibold text-foreground">用户名 (可选)</Text>
              <TextInput
                value={username}
                onChangeText={setUsername}
                placeholder="如果源需要认证"
                placeholderTextColor={colors.muted}
                className="bg-surface border border-border rounded-lg px-4 py-3 text-foreground"
              />
            </View>

            {/* Password */}
            <View className="gap-2">
              <Text className="text-sm font-semibold text-foreground">密码 (可选)</Text>
              <TextInput
                value={password}
                onChangeText={setPassword}
                placeholder="如果源需要认证"
                placeholderTextColor={colors.muted}
                secureTextEntry
                className="bg-surface border border-border rounded-lg px-4 py-3 text-foreground"
              />
            </View>

            {/* Enabled Toggle */}
            <View className="flex-row items-center justify-between bg-surface rounded-lg p-4 border border-border">
              <Text className="text-sm font-semibold text-foreground">启用此源</Text>
              <Pressable
                onPress={() => setEnabled(!enabled)}
                style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }]}
              >
                <View
                  className={`w-12 h-6 rounded-full items-center justify-center ${
                    enabled ? "bg-success" : "bg-muted"
                  }`}
                >
                  <View
                    className="w-5 h-5 rounded-full bg-white"
                    style={{
                      transform: [{ translateX: enabled ? 8 : -8 }],
                    }}
                  />
                </View>
              </Pressable>
            </View>

            {/* Test Connection */}
            <TouchableOpacity
              className="bg-primary bg-opacity-10 rounded-lg py-3 px-4 flex-row items-center justify-center gap-2"
              onPress={handleTestConnection}
              disabled={testing}
            >
              <MaterialIcons name="cloud" size={20} color="#0066CC" />
              <Text className="text-sm font-semibold text-primary">
                {testing ? "测试中..." : "测试连接"}
              </Text>
            </TouchableOpacity>

            {/* Test Result */}
            {testResult && (
              <View
                className={`rounded-lg p-3 flex-row items-center gap-2 ${
                  testResult.success
                    ? "bg-success bg-opacity-10"
                    : "bg-error bg-opacity-10"
                }`}
              >
                <MaterialIcons
                  name={testResult.success ? "check-circle" : "error"}
                  size={20}
                  color={testResult.success ? "#22C55E" : "#EF4444"}
                />
                <Text
                  className={`text-sm font-medium ${
                    testResult.success ? "text-success" : "text-error"
                  }`}
                >
                  {testResult.message}
                </Text>
              </View>
            )}
          </View>

          {/* Actions */}
          <View className="flex-row gap-3 mt-4">
            <TouchableOpacity
              className="flex-1 bg-surface border border-border rounded-lg py-3 items-center"
              onPress={() => router.back()}
            >
              <Text className="text-sm font-semibold text-foreground">取消</Text>
            </TouchableOpacity>
            <TouchableOpacity
              className="flex-1 bg-primary rounded-lg py-3 items-center"
              onPress={handleSave}
            >
              <Text className="text-sm font-semibold text-white">保存</Text>
            </TouchableOpacity>
          </View>
        </View>
      </ScrollView>
    </ScreenContainer>
  );
}
