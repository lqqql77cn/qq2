import { ScrollView, Text, View, TouchableOpacity, Pressable, Alert } from "react-native";
import { useRouter } from "expo-router";
import { ScreenContainer } from "@/components/screen-container";
import { useDownload } from "@/lib/contexts/download-context";
import { useColors } from "@/hooks/use-colors";
import { MaterialIcons } from "@expo/vector-icons";
import { useState } from "react";

export default function SourcesScreen() {
  const router = useRouter();
  const colors = useColors();
  const { state, deleteSource } = useDownload();
  const [editingId, setEditingId] = useState<string | null>(null);

  const handleAddSource = () => {
    router.push("/source-form" as any);
  };

  const handleEditSource = (sourceId: string) => {
    router.push({
      pathname: "/source-form",
      params: { sourceId },
    } as any);
  };

  const handleDeleteSource = (sourceId: string, sourceName: string) => {
    Alert.alert(
      "删除下载源",
      `确定要删除下载源 "${sourceName}" 吗？`,
      [
        { text: "取消", onPress: () => {}, style: "cancel" },
        {
          text: "删除",
          onPress: () => {
            deleteSource(sourceId);
          },
          style: "destructive",
        },
      ]
    );
  };

  const getSourceTypeLabel = (type: string) => {
    const labels: Record<string, string> = {
      apt: "APT (Debian/Ubuntu)",
      yum: "YUM (RedHat/CentOS)",
      dnf: "DNF (Fedora)",
      custom: "自定义",
    };
    return labels[type] || type;
  };

  return (
    <ScreenContainer className="p-4">
      <ScrollView contentContainerStyle={{ flexGrow: 1 }} showsVerticalScrollIndicator={false}>
        <View className="gap-4">
          {/* Header */}
          <View className="flex-row items-center justify-between mb-2">
            <View>
              <Text className="text-2xl font-bold text-foreground">下载源管理</Text>
              <Text className="text-sm text-muted mt-1">已配置 {state.sources.length} 个源</Text>
            </View>
            <Pressable
              onPress={handleAddSource}
              style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }]}
            >
              <View className="bg-primary rounded-full w-12 h-12 items-center justify-center">
                <MaterialIcons name="add" size={24} color="white" />
              </View>
            </Pressable>
          </View>

          {/* Sources List */}
          {state.sources.length === 0 ? (
            <View className="items-center justify-center py-12 gap-3">
              <MaterialIcons name="cloud-off" size={48} color={colors.muted} />
              <Text className="text-base font-semibold text-foreground">暂无下载源</Text>
              <Text className="text-sm text-muted text-center">点击下方按钮添加第一个下载源</Text>
              <TouchableOpacity
                className="bg-primary rounded-lg py-2 px-6 mt-2"
                onPress={handleAddSource}
              >
                <Text className="text-white font-semibold text-sm">添加下载源</Text>
              </TouchableOpacity>
            </View>
          ) : (
            <View className="gap-3">
              {state.sources.map((source) => (
                <View
                  key={source.id}
                  className="bg-surface rounded-lg p-4 border border-border"
                >
                  <View className="flex-row items-start justify-between mb-2">
                    <View className="flex-1">
                      <View className="flex-row items-center gap-2 mb-1">
                        <Text className="text-base font-semibold text-foreground flex-1">
                          {source.name}
                        </Text>
                        {source.enabled ? (
                          <View className="bg-success bg-opacity-20 rounded-full px-2 py-1">
                            <Text className="text-xs font-semibold text-success">启用</Text>
                          </View>
                        ) : (
                          <View className="bg-muted bg-opacity-20 rounded-full px-2 py-1">
                            <Text className="text-xs font-semibold text-muted">禁用</Text>
                          </View>
                        )}
                      </View>
                      <Text className="text-xs text-muted mb-2">
                        {getSourceTypeLabel(source.type)}
                      </Text>\n                      <Text className="text-xs text-muted break-words">{source.url}</Text>
                    </View>
                  </View>

                  {/* Actions */}
                  <View className="flex-row gap-2 mt-3 pt-3 border-t border-border">
                    <Pressable
                      onPress={() => handleEditSource(source.id)}
                      style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }, { flex: 1 }]}
                    >
                      <View className="flex-row items-center justify-center gap-2 py-2 px-3 bg-primary bg-opacity-10 rounded-lg">
                        <MaterialIcons name="edit" size={16} color="#0066CC" />
                        <Text className="text-sm font-semibold text-primary">编辑</Text>
                      </View>
                    </Pressable>
                    <Pressable
                      onPress={() => handleDeleteSource(source.id, source.name)}
                      style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }, { flex: 1 }]}
                    >
                      <View className="flex-row items-center justify-center gap-2 py-2 px-3 bg-error bg-opacity-10 rounded-lg">
                        <MaterialIcons name="delete" size={16} color="#EF4444" />
                        <Text className="text-sm font-semibold text-error">删除</Text>
                      </View>
                    </Pressable>
                  </View>
                </View>
              ))}
            </View>
          )}
        </View>
      </ScrollView>
    </ScreenContainer>
  );
}
