// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "GameDataName.generated.h"

// #include "GameSystem/GameDataName.h"

typedef FName GameDataCategoryName;
typedef FName GameDataName;

//声明分类名称
#define DEF_CATEGORYNAME(CategoryName)\
static GameDataCategoryName CategoryName(TEXT(#CategoryName))\
//声明数据名称
#define DEF_NAME(Name)\
static GameDataName Name(TEXT(#Name))\

//获取UI分类名称
#define GET_UI_CATEGORYNAME(CategoryName)\
GameDataNameAsset::UI_##CategoryName::CategoryName\
//获取分类中的数据名称
#define GET_UI_NAME(CategoryName,Name)\
GameDataNameAsset::UI_##CategoryName::Name\
//获取分类名称
#define GET_DEF_CATEGORYNAME(CategoryName)\
GameDataNameAsset::GameDataAsset_GlobalAsset_##CategoryName::GlobalAsset_##CategoryName\
//获取分类中的数据名称
#define GET_DEF_NAME(CategoryName,Name)\
GameDataNameAsset::GameDataAsset_GlobalAsset_##CategoryName::Name\

//获取移动分类名称
#define GET_MOVE_CATEGORYNAME(CategoryName)\
MoveTemp(GET_DEF_CATEGORYNAME(CategoryName))\
//获取移动分类中的数据名称
#define GET_MOVE_NAME(CategoryName,Name)\
MoveTemp(GET_DEF_NAME(CategoryName,Name))\

/**
 * 全局资源名称
 *
 *  如果需要修改，请修改【蓝图实现BP_xxxx_CateName】【C++的UI宏名称】
 */
 /*
	 UI的分类名称

	 如果需要修改，请修改【数据表DT_GameUserInterface】【蓝图实现BP_xxxx_CateName】【C++的UI宏名称】

	 资产分为：数据资产和UI资产
	 介绍：	1.数据资产是数据管理系统使用的资产名称
			2.UI资产是UI管理系统使用的资产名称，路径：GlobalAsset_UI - UserInter 数据表中

 */
 //游戏数据名字资产
namespace GameDataNameAsset {

	//数据资产【UI】
	namespace GameDataAsset_GlobalAsset_UI {
		//数据资产——UI分类
		DEF_CATEGORYNAME(GlobalAsset_UI);
		//用户接口
		DEF_NAME(UserInter);
	}

	//数据资产【Player】角色
	namespace GameDataAsset_GlobalAsset_Player {
		//数据资产——Player分类
		DEF_CATEGORYNAME(GlobalAsset_Player);
		//角色更新
		DEF_NAME(Update);
	}

	//数据资产【Dialogue】剧情
	namespace GameDataAsset_GlobalAsset_Dialogue {
		//数据资产——剧情分类
		DEF_CATEGORYNAME(GlobalAsset_Dialogue);
		//章节关联名称
		DEF_NAME(Chapter);
		//主线剧情对话列表
		DEF_NAME(MainLine);
		//支线剧情对话列表
		DEF_NAME(OtherLine);
	}

	//数据资产【Card】卡片
	namespace GameDataAsset_GlobalAsset_Card {
		//数据资产——Card卡片分类
		DEF_CATEGORYNAME(GlobalAsset_Card);
		//攻击卡
		DEF_NAME(ATK);
		//生产卡
		DEF_NAME(Spawn);
		//防御卡
		DEF_NAME(Defence);
		//功能卡
		DEF_NAME(Function);
		//范围攻击卡
		DEF_NAME(RangeATK);
	}

	//数据资产【Material】材料
	namespace GameDataAsset_GlobalAsset_Material {
		//数据资产——Material材料分类
		DEF_CATEGORYNAME(GlobalAsset_Material);
		//卡片配方
		DEF_NAME(Blueprint);
		//卡片配方材料
		DEF_NAME(BlueprintMater);
		//转职
		DEF_NAME(Change);
		//香料
		DEF_NAME(Spices);
		//四叶草
		DEF_NAME(Clover);
		//防御卡技能书
		DEF_NAME(SkillBook);
		//兑换券
		DEF_NAME(Ticket);
		//强化水晶
		DEF_NAME(Crystal);
		//钻头
		DEF_NAME(Bit);
		//关卡钥匙和徽章
		DEF_NAME(LevelKey);
		//金卡进化材料匹配表格
		DEF_NAME(GoldCardUpData);
		//防御卡技能书匹配表格
		DEF_NAME(SkillBookData);
	}

	//数据资产【Equip】装备
	namespace GameDataAsset_GlobalAsset_Equip {
		//数据资产——Equip装备分类
		DEF_CATEGORYNAME(GlobalAsset_Equip);
		//背包
		DEF_NAME(Bag);
		//礼盒
		DEF_NAME(GiftBox);
		//主武器
		DEF_NAME(WeaponFirst);
		//副武器
		DEF_NAME(WeaponSecond);
		//超级武器
		DEF_NAME(WeaponSuper);
		//武器宝石
		DEF_NAME(WeaponGem);
		//男-身体
		DEF_NAME(BodyMan);
		//女-身体
		DEF_NAME(BodyMen);
		//女-头发
		DEF_NAME(HairMen);
		//女-眼镜
		DEF_NAME(GlassesMen);
		//女-帽子
		DEF_NAME(CapMen);
		//不区分-帽子
		DEF_NAME(CapBase);
		//女-眼睛
		DEF_NAME(EyeMen);
		//不区分-眼睛
		DEF_NAME(EyeBase);
		//女-脸
		DEF_NAME(FaceMen);
		//男-翅膀
		DEF_NAME(FlyItemMan);
		//女-翅膀
		DEF_NAME(FlyItemMen);
		//不区分-翅膀
		DEF_NAME(FlyItemBase);
		//男-套装
		DEF_NAME(SuitMan);
		//女-套装
		DEF_NAME(SuitMen);
		//不区分-套装
		DEF_NAME(SuitBase);
	}

	//数据资产【Mail】邮件
	namespace GameDataAsset_GlobalAsset_Mail {

		//数据资产——Equip装备分类
		DEF_CATEGORYNAME(GlobalAsset_Mail);

		//邮件
		DEF_NAME(Mail);
	}

	//数据资产【GameVS】关卡进行
	namespace GameDataAsset_GlobalAsset_GameVS {
		//数据资产——【GameVS】关卡进行分类
		DEF_CATEGORYNAME(GlobalAsset_GameVS);
		//Buff
		DEF_NAME(Buff);
		//地形放置卡片
		DEF_NAME(LevelPlayPreCard);
		//游戏角色
		DEF_NAME(GamePlayer);

	}

	//数据资产【任务】
	namespace GameDataAsset_GlobalAsset_Task {
		//数据资产——任务分类
		DEF_CATEGORYNAME(GlobalAsset_Task);
		//主线任务
		DEF_NAME(MainTask);
		//每日任务
		DEF_NAME(DayTask);
		//挑战任务
		DEF_NAME(ChallengeTask);
		//角色升级任务
		DEF_NAME(PlayerUpTask);



		//任务实现的数据表

		//关卡任务
		DEF_NAME(Imp_Task_Level);
		//卡片制作
		DEF_NAME(Imp_Task_MakeCard);
		//卡片强化
		DEF_NAME(Imp_Task_CardUpgrade);
		//技能书
		DEF_NAME(Imp_Task_CardSkillBooks);
		//宝石
		DEF_NAME(Imp_Task_Gem);
		//情报岛
		DEF_NAME(Imp_Task_Information);
		//角色升级
		DEF_NAME(Imp_Task_PlayerUpgrade);
	}


	//数据资产【Texture】纹理，一般是常用纹理
	namespace GameDataAsset_GlobalAsset_Texture {
		//数据资产——【Texture】纹理
		DEF_CATEGORYNAME(GlobalAsset_Texture);
		//物品的等级 1-16级，小图标
		DEF_NAME(ItemLevelLog);
		//货币【金币，礼券，点券，威望等等】
		DEF_NAME(Coin);
		//玩家等级纹理
		DEF_NAME(PlayerLevelTexture);
	}

	//数据资产【Instruction】指令，游戏指令
	namespace GameDataAsset_GlobalAsset_Instruction {
		//数据资产——【Texture】纹理
		DEF_CATEGORYNAME(GlobalAsset_Instruction);
		//全部可用的指令
		DEF_NAME(All);
		//用于游戏场景的指令（GameVs）
		DEF_NAME(Game);
		//适用于角色存档的指令
		DEF_NAME(Player);
		//适用于礼盒模式的指令
		DEF_NAME(Gift);
		//适用于符合引用作为关键映射
		DEF_NAME(SymbolRef);
		//适用于符号的指令
		DEF_NAME(Symbol);
	}

	//数据资产【Data】一些特别的数据
	namespace GameDataAsset_GlobalAsset_Data {
		//数据资产——【Texture】纹理
		DEF_CATEGORYNAME(GlobalAsset_Data);
		//游戏道具的ID号集合
		DEF_NAME(GameID);
		//游戏道具的最大ID号和已经被删除的ID号
		DEF_NAME(ItemID);
		//装备道具整理类型数据表
		DEF_NAME(EquipArrange);
		//卡片道具整理类型数据表
		DEF_NAME(CardArrange);
		//材料道具整理类型数据表
		DEF_NAME(MaterialArrange);
	}

	//UI资产 全局UI
	namespace UI_Global {
		//全局UI分类
		DEF_CATEGORYNAME(Global);
		//情报岛UI
		DEF_NAME(GameInfor);
		//任务UI
		DEF_NAME(GameTask);
		//任务完成提示
		DEF_NAME(TaskFinishTip);
		//解锁新关卡提示
		DEF_NAME(NewLevelTip);
		//新物品显示
		DEF_NAME(NewItemShow);
		//剧情对话信息
		DEF_NAME(DialogueInfor);
		//城镇UI操作主面板
		DEF_NAME(MainFrame);
		//世界地图UI
		DEF_NAME(GameWorldMap);
		//公告
		DEF_NAME(Notice);
		//设置UI
		DEF_NAME(GameSetting);
		//打开游戏【视频UI】
		DEF_NAME(OpenGame);
		//进入游戏主界面
		DEF_NAME(InGame);
		//测试版本的提示文字
		DEF_NAME(GameTest);
		//VS准备界面
		DEF_NAME(GamePrepare);
		// Windows平台鼠标移入时显示的物品文字
		DEF_NAME(ItemTitleTip);
		//合成屋
		DEF_NAME(Synthesis);
		//剧情
		DEF_NAME(Dialogue);
		//邮件
		DEF_NAME(Mail);
		//玩家背包
		DEF_NAME(PlayerBag);
		//支付界面
		DEF_NAME(Pay);
		//选择提示UI
		DEF_NAME(SelectTip);
		//未解锁的物品提示UI
		DEF_NAME(NotUnlockedItemTip);	
	};

	//UI资产 角色背包UI
	namespace UI_PlayerBag {
		//角色背包分类
		DEF_CATEGORYNAME(PlayerBag);
		//卡片格子
		DEF_NAME(CardGrid);
		//装备格子
		DEF_NAME(EquipmentGrid);
		//材料格子
		DEF_NAME(MaterialGrid);
		//角色形象显示
		DEF_NAME(PlayerShow);
		//物品的展示信息
		DEF_NAME(ItemTip);
		//卡片物品的展示信息
		DEF_NAME(ItemCardTip);
		//物品的展示信息【礼包(开箱子)专属界面】
		DEF_NAME(GiftItemTip);
		//物品的展示信息【礼包(开箱子)概率项目展示界面】
		DEF_NAME(GiftItemTip_Item);
		//装备格子-定制格子-背包道具专属
		DEF_NAME(Equipment_Bag_Grid);
		//角色信息面板
		DEF_NAME(PlayerInforPanel);
		//物品排序设置UI
		DEF_NAME(Arrangement);
		//物品排序项目
		DEF_NAME(ArrangementItem);
		//物品排序项目设置细节面板
		DEF_NAME(ArrangementSetting);
		//技能书选项卡标签
		DEF_NAME(SkillBookTab);
	}

	//UI资产 设置UI
	namespace UI_Setting
	{
		//设置UI分类
		DEF_CATEGORYNAME(Setting);
		//设置UI选项卡
		DEF_NAME(SettingButtonTab);
	}

	//UI资产 地图UI
	namespace UI_WorldMap {
		//地图UI分类
		DEF_CATEGORYNAME(WorldMap);
		//城镇地图
		DEF_NAME(MainMap);
		//美味岛
		DEF_NAME(Map1);
		//火山岛
		DEF_NAME(Map2);
		//火山遗迹
		DEF_NAME(Map3);
		//浮空岛
		DEF_NAME(Map4);
		//海底
		DEF_NAME(Map5);
	}

	//UI资产 游戏战斗UI
	namespace UI_GameVs
	{
		//设置UI分类
		DEF_CATEGORYNAME(GameVs);
		//放置角色的提升UI
		DEF_NAME(PlayPlayerTip);
		//底部文字提示
		DEF_NAME(BottomTextTip);
		//关卡倒计时
		DEF_NAME(GameTimer);
		//游戏结束结算界面
		DEF_NAME(GameOver);
		//角色头像
		DEF_NAME(GamePlayerHead);
		//卡片栏
		DEF_NAME(CardBar);
		//卡片UI
		DEF_NAME(CardUI);
	}


	//UI资产 剧情对话UI
	namespace UI_Dialogue
	{
		//设置UI分类
		DEF_CATEGORYNAME(Dialogue);
		//对话选项
		DEF_NAME(DialogueSelect);
	}

	//UI资产 邮件UI
	namespace UI_Mail
	{
		//邮件UI分类
		DEF_CATEGORYNAME(Mail);
		//邮件列表项目
		DEF_NAME(MessageBox);
		//邮件领取邮件的界面
		DEF_NAME(GiftBox);
	}

	//UI资产 提示UI
	namespace UI_Tip {
		//提示UI分类
		DEF_CATEGORYNAME(Tip);
		//玩家获得道具的提示框
		DEF_NAME(PlayerItemTipBox);
		//玩家获得道具的提示框-提示框项目
		DEF_NAME(PlayerItemTipBoxItem);
		//描述提示界面
		DEF_NAME(DesTip);
	}

	//UI资产 合成屋UI
	namespace UI_Synthesis {
		//合成屋UI
		DEF_CATEGORYNAME(Synthesis);
		//合成屋道具格子-道具
		DEF_NAME(SynItemGrid);
		//合成屋道具格子-卡片
		DEF_NAME(SynCardGrid);
		//卡片制作主界面
		DEF_NAME(MakeCard);
		//卡片强化主界面
		DEF_NAME(UpgradeCard);
		//卡片转职主界面
		DEF_NAME(TransferCard);
		//武器宝石镶嵌界面
		DEF_NAME(WeaponAndGem);
		//宝石强化主界面
		DEF_NAME(UpgradeGem);
		//宝石分解主界面
		DEF_NAME(SplitGem);
		//金卡进化主界面
		DEF_NAME(GoldCardEvolve);
	}
	
}



//获取UI的方法
// UGameSystemFunction::GetUserInterClassByName ......


UCLASS()
class FVM_API UGameDataName : public UObject {
	GENERATED_BODY()
public:
	//Tag资产全局名称
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset UI"))
	static FName GetAsset_UI() { return GET_DEF_CATEGORYNAME(UI); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset Card"))
	static FName GetAsset_Card() { return GET_DEF_CATEGORYNAME(Card); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset Equip"))
	static FName GetAsset_Equip() { return GET_DEF_CATEGORYNAME(Equip); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset GameVS"))
	static FName GetAsset_GameVS() { return GET_DEF_CATEGORYNAME(GameVS); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset Material"))
	static FName GetAsset_Material() { return GET_DEF_CATEGORYNAME(Material); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset Player"))
	static FName GetAsset_Player() { return GET_DEF_CATEGORYNAME(Player); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset Task"))
	static FName GetAsset_Task() { return GET_DEF_CATEGORYNAME(Task); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name Global Asset Dialogue"))
	static FName GetAsset_Dialogue() { return GET_DEF_CATEGORYNAME(Dialogue); }
public:
	//Tag全局UI
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI Global"))
	static FName Get_UI_Global() { return GET_UI_CATEGORYNAME(Global); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global GameInfor"))
	static FName Get_UI_GameInfor() { return GET_UI_NAME(Global, GameInfor); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global DialogueInfor"))
	static FName Get_UI_DialogueInfor() { return GET_UI_NAME(Global, DialogueInfor); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global GamePrepare"))
	static FName Get_UI_GamePrepare() { return GET_UI_NAME(Global, GamePrepare); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global GameSetting"))
	static FName Get_UI_GameSetting() { return GET_UI_NAME(Global, GameSetting); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global GameTask"))
	static FName Get_UI_GameTask() { return GET_UI_NAME(Global, GameTask); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global GameTest"))
	static FName Get_UI_GameTest() { return GET_UI_NAME(Global, GameTest); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global GameWorldMap"))
	static FName Get_UI_GameWorldMap() { return GET_UI_NAME(Global, GameWorldMap); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global InGame"))
	static FName Get_UI_InGame() { return GET_UI_NAME(Global, InGame); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global ItemTitleTip"))
	static FName Get_UI_ItemTitleTip() { return GET_UI_NAME(Global, ItemTitleTip); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global MainFrame"))
	static FName Get_UI_MainFrame() { return GET_UI_NAME(Global, MainFrame); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global NewItemShow"))
	static FName Get_UI_NewItemShow() { return GET_UI_NAME(Global, NewItemShow); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global NewLevelTip"))
	static FName Get_UI_NewLevelTip() { return GET_UI_NAME(Global, NewLevelTip); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global Notice"))
	static FName Get_UI_Notice() { return GET_UI_NAME(Global, Notice); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global OpenGame"))
	static FName Get_UI_OpenGame() { return GET_UI_NAME(Global, OpenGame); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName_UI Global Synthesis"))
	static FName Get_UI_Synthesis() { return GET_UI_NAME(Global, Synthesis); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global TaskFinishTip"))
	static FName Get_UI_TaskFinishTip() { return GET_UI_NAME(Global, TaskFinishTip); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global Dialogue"))
	static FName Get_UI_Dialogue() { return GET_UI_NAME(Global, Dialogue); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global Mail"))
	static FName Get_UI_MailView() { return GET_UI_NAME(Global, Mail); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global PlayerBag"))
	static FName Get_UI_PlayerBagView() { return GET_UI_NAME(Global, PlayerBag); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global Pay"))
	static FName Get_UI_PayUI() { return GET_UI_NAME(Global, Pay); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Global SelectTip"))
	static FName Get_UI_SelectTipUI() { return GET_UI_NAME(Global, SelectTip); }
public:
	//Tag角色背包UI
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI PlayerBag"))
	static FName Get_UI_PlayerBag() { return GET_UI_CATEGORYNAME(PlayerBag); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI PlayerBag CardGrid"))
	static FName Get_UI_CardGrid() { return GET_UI_NAME(PlayerBag, CardGrid); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI PlayerBag EquipmentGrid"))
	static FName Get_UI_EquipmentGrid() { return GET_UI_NAME(PlayerBag, EquipmentGrid); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI PlayerBag ItemTip"))
	static FName Get_UI_ItemTip() { return GET_UI_NAME(PlayerBag, ItemTip); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI PlayerBag MaterialGrid"))
	static FName Get_UI_MaterialGrid() { return GET_UI_NAME(PlayerBag, MaterialGrid); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI PlayerBag PlayerShow"))
	static FName Get_UI_PlayerShow() { return GET_UI_NAME(PlayerBag, PlayerShow); }
public:
	//Tag设置UI
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI Setting"))
	static FName Get_UI_Setting() { return GET_UI_CATEGORYNAME(Setting); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Setting SettingButtonTab"))
	static FName Get_UI_SettingButtonTab() { return GET_UI_NAME(Setting, SettingButtonTab); }
public:
	//Tag世界地图UI
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI WorldMap"))
	static FName Get_UI_WorldMap() { return GET_UI_CATEGORYNAME(WorldMap); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI WorldMap MainMap"))
	static FName Get_UI_MainMap() { return GET_UI_NAME(WorldMap, MainMap); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI WorldMap Map1"))
	static FName Get_UI_Map1() { return GET_UI_NAME(WorldMap, Map1); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI WorldMap Map2"))
	static FName Get_UI_Map2() { return GET_UI_NAME(WorldMap, Map2); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI WorldMap Map3"))
	static FName Get_UI_Map3() { return GET_UI_NAME(WorldMap, Map3); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI WorldMap Map4"))
	static FName Get_UI_Map4() { return GET_UI_NAME(WorldMap, Map4); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI WorldMap Map5"))
	static FName Get_UI_Map5() { return GET_UI_NAME(WorldMap, Map5); }
public:
	//Tag关卡UI
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI GameVs"))
	static FName Get_UI_GameVs() { return GET_UI_CATEGORYNAME(GameVs); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI GameVs BottomTextTip"))
	static FName Get_UI_BottomTextTip() { return GET_UI_NAME(GameVs, BottomTextTip); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI GameVs GameOver"))
	static FName Get_UI_GameOver() { return GET_UI_NAME(GameVs, GameOver); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI GameVs GamePlayerHead"))
	static FName Get_UI_GamePlayerHead() { return GET_UI_NAME(GameVs, GamePlayerHead); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI GameVs GameTimer"))
	static FName Get_UI_GameTimer() { return GET_UI_NAME(GameVs, GameTimer); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI GameVs PlayPlayerTip"))
	static FName Get_UI_PlayPlayerTip() { return GET_UI_NAME(GameVs, PlayPlayerTip); }
public:
	//Tag任务
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName Task MainTask"))
	static FName Get_Task_MainTask() { return GET_DEF_NAME(Task, MainTask); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName Task DayTask"))
	static FName Get_Task_DayTask() { return GET_DEF_NAME(Task, DayTask); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName Task ChallengeTask"))
	static FName Get_Task_ChallengeTask() { return GET_DEF_NAME(Task, ChallengeTask); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName Task PlayerUpTask"))
	static FName Get_Task_PlayerUpTask() { return GET_DEF_NAME(Task, PlayerUpTask); }
public:
	//Tag UI 对话
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI Dialogue"))
	static FName Get_UI_DialogueView() { return GET_UI_CATEGORYNAME(Dialogue); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Dialogue DialogueSelect"))
	static FName Get_UI_DialogueSelect() { return GET_UI_NAME(Dialogue, DialogueSelect); }

public:
	//Tag UI 邮件
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI Mail"))
	static FName Get_UI_Mail() { return GET_UI_CATEGORYNAME(Mail); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Mail MessageBox"))
	static FName Get_UI_MailBox() { return GET_UI_NAME(Mail, MessageBox); }

public:
	//Tag UI 提示
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Category Name UI Tip"))
	static FName Get_UI_Tip() { return GET_UI_CATEGORYNAME(Tip); }
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get DataName UI Tip DesTip"))
	static FName Get_UI_Tip_DesTip() { return GET_UI_NAME(Tip, DesTip); }
};