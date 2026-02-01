import { ScrollView, Text, View, TouchableOpacity, Pressable } from "react-native";
import { useRouter } from "expo-router";
import { ScreenContainer } from "@/components/screen-container";
import { useDownload } from "@/lib/contexts/download-context";
import { useColors } from "@/hooks/use-colors";
import { MaterialIcons } from "@expo/vector-icons";
import { cn } from "@/lib/utils";

interface MenuCard {
  id: string;
  icon: string;
  title: string;
  description: string;
  route: string;
  color: string;
}

export default function HomeScreen() {
  const router = useRouter();
  const colors = useColors();
  const { state } = useDownload();

  const menuCards: MenuCard[] = [
    {
      id: "sources",
      icon: "cloud-download",
      title: "下载源管理",
      description: `已配置 ${state.sources.length} 个源`,
      route: "/sources",
      color: "#0066CC",
    },
    {
      id: "packages",
      icon: "list",
      title: "软件清单",
      description: `已选择 ${state.selectedPackages.length} 个软件`,
      route: "/packages",
      color: "#00BFFF",
    },
    {
      id: "settings",
      icon: "settings",
      title: "下载设置",
      description: `目标: ${state.settings.targetSystem} ${state.settings.targetArchitecture}`,
      route: "/settings",
      color: "#1E90FF",
    },
    {
      id: "download",
      icon: "download",
      title: "打包与下载",
      description: "开始下载和打包",
      route: "/download",
      color: "#0066CC",
    },
  ];

  const handleCardPress = (route: string) => {
    router.push(route as any);
  };

  return (
    <ScreenContainer className="p-4">
      <ScrollView contentContainerStyle={{ flexGrow: 1 }} showsVerticalScrollIndicator={false}>
        <View className="gap-6">
          {/* Header */}
          <View className="gap-2">
            <Text className="text-3xl font-bold text-foreground">银河麒麟下载助手</Text>
            <Text className="text-base text-muted">为离线系统打包软件的便捷工具</Text>
          </View>

          {/* Quick Stats */}
          <View className="flex-row gap-3">
            <View className="flex-1 bg-surface rounded-lg p-4 border border-border">
              <Text className="text-xs text-muted mb-1">下载源</Text>
              <Text className="text-2xl font-bold text-foreground">{state.sources.length}</Text>
            </View>
            <View className="flex-1 bg-surface rounded-lg p-4 border border-border">
              <Text className="text-xs text-muted mb-1">已选软件</Text>
              <Text className="text-2xl font-bold text-foreground">{state.selectedPackages.length}</Text>
            </View>
            <View className="flex-1 bg-surface rounded-lg p-4 border border-border">
              <Text className="text-xs text-muted mb-1">目标架构</Text>
              <Text className="text-base font-semibold text-foreground">{state.settings.targetArchitecture}</Text>
            </View>
          </View>

          {/* Menu Cards */}
          <View className="gap-3">
            {menuCards.map((card) => (
              <Pressable
                key={card.id}
                onPress={() => handleCardPress(card.route)}
                style={({ pressed }) => [{ opacity: pressed ? 0.7 : 1 }]}
              >
                <View className="bg-surface rounded-lg p-4 border border-border flex-row items-center gap-4">
                  <View
                    className="w-12 h-12 rounded-lg items-center justify-center"
                    style={{ backgroundColor: card.color + "20" }}
                  >
                    <MaterialIcons name={card.icon as any} size={24} color={card.color} />
                  </View>
                  <View className="flex-1">
                    <Text className="text-base font-semibold text-foreground">{card.title}</Text>
                    <Text className="text-sm text-muted mt-1">{card.description}</Text>
                  </View>
                  <MaterialIcons name="chevron-right" size={24} color={colors.muted} />
                </View>
              </Pressable>
            ))}
          </View>

          {/* Quick Start Section */}
          {state.sources.length === 0 && (
            <View className="bg-primary bg-opacity-10 rounded-lg p-4 border border-primary border-opacity-20">
              <View className="flex-row gap-3 mb-3">
                <MaterialIcons name="info" size={20} color="#0066CC" />
                <Text className="flex-1 text-sm font-semibold text-foreground">快速开始</Text>
              </View>
              <Text className="text-sm text-muted leading-relaxed mb-4">
                欢迎使用银河麒麟软件下载助手。请先配置至少一个下载源，然后选择需要的软件进行下载。
              </Text>
              <TouchableOpacity
                className="bg-primary rounded-lg py-2 px-4 items-center"
                onPress={() => handleCardPress("/sources")}
              >
                <Text className="text-white font-semibold text-sm">前往配置下载源</Text>
              </TouchableOpacity>
            </View>
          )}

          {/* Download History */}
          {state.downloadHistory.length > 0 && (
            <View className="gap-2">
              <Text className="text-base font-semibold text-foreground">最近下载</Text>
              {state.downloadHistory.slice(0, 3).map((history) => (
                <View key={history.id} className="bg-surface rounded-lg p-3 border border-border">
                  <Text className="text-sm font-medium text-foreground">
                    {history.packageNames.join(", ")}
                  </Text>
                  <Text className="text-xs text-muted mt-1">
                    {new Date(history.timestamp).toLocaleString("zh-CN")}
                  </Text>
                </View>
              ))}
            </View>
          )}
        </View>
      </ScrollView>
    </ScreenContainer>
  );
}
