// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/Item/ItemIdDataTable.h"

#if WITH_EDITOR
#include "UObject/Script.h"
#include "ScopedTransaction.h"
#include "UnrealEd/Public/DataTableEditorUtils.h"
#include "Internationalization/Internationalization.h"

#define LOCTEXT_NAMESPACE "UItemIdEditorManager"

#endif

UItemIdEditorManager* UItemIdEditorManager::IdEditorOperate = nullptr;

UItemIdEditorManager* UItemIdEditorManager::GetItemIdEditorManager() {
#if WITH_EDITOR
	if (UItemIdEditorManager::IdEditorOperate)
	{
		return UItemIdEditorManager::IdEditorOperate;
	}

	UItemIdEditorManager::IdEditorOperate = NewObject<UItemIdEditorManager>();
	UItemIdEditorManager::IdEditorOperate->AddToRoot();
	return UItemIdEditorManager::IdEditorOperate;
#endif

	return nullptr;
}

UDataTable* UItemIdEditorManager::GetItemIdDataTable() {
	if (IsValid(this->ItemIdDataTablePtr))
	{
		return this->ItemIdDataTablePtr;
	}

	FSoftObjectPath DataTablePath = FString("DataTable'/Game/Resource/BP/Data/ItemIdManager/DT_GameID.DT_GameID'");

	this->ItemIdDataTablePtr = Cast<UDataTable>(DataTablePath.TryLoad());

	return this->ItemIdDataTablePtr;
}

UDataTable* UItemIdEditorManager::GetIdDataTable() {

	if (IsValid(this->ItemIdPtr))
	{
		return this->ItemIdPtr;
	}

	FSoftObjectPath DataTablePath = FString("DataTable'/Game/Resource/BP/Data/ItemIdManager/DT_ItemID.DT_ItemID'");

	this->ItemIdPtr = Cast<UDataTable>(DataTablePath.TryLoad());

	return this->ItemIdPtr;
}

int32 UItemIdEditorManager::GetNewID() {
	if (!IsValid(this->GetIdDataTable()))
	{
		return -1;
	}

	for (const FName& RowName : ItemIdPtr->GetRowNames())
	{
		FItemId* Id = ItemIdPtr->FindRow<FItemId>(RowName, TEXT("GetIDRow"));
		if (Id) {

			int32 DeID = this->GetDeleteID();
			if (DeID != -1)
			{
				return DeID;
			}

			return Id->MaxID + 1;
		}

		return -1;
	}

	return -1;
}

void UItemIdEditorManager::RemoveID(FString ItemName) {

	if (!IsValid(this->GetItemIdDataTable()))
	{
		return;
	}

	for (const FName& RowName : ItemIdDataTablePtr->GetRowNames())
	{
		FItemIdDataTable* Item = ItemIdDataTablePtr->FindRow<FItemIdDataTable>(RowName,
			TEXT("GetRemoveID"));
		if (Item) {

			//获取当前移除的ID号
			int32* ReID = Item->Item.Find(ItemName);
			if (ReID)
			{
				UE_LOG(LogTemp, Warning, TEXT("道具[%s]删除成功"), *ItemName);
				Item->Item.Remove(ItemName);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("道具[%s]删除失败"), *ItemName);
			}

			this->AddDeleteID(*ReID);

			return;
		}

		return;
	}

}

void UItemIdEditorManager::AddID(FString ItemName) {

	if (!IsValid(this->GetItemIdDataTable()) || !IsValid(this->GetIdDataTable()))
	{
		return;
	}

	for (const FName& RowName : ItemIdDataTablePtr->GetRowNames())
	{
		FItemIdDataTable* Item = ItemIdDataTablePtr->FindRow<FItemIdDataTable>(RowName, TEXT("AddID"));

		if (Item) {

			int32 DeID = this->GetDeleteID();
			if (DeID != -1)
			{
				//移除ID
				this->RemoveDeleteID(DeID);
				//新增ID
				Item->Item.Emplace(ItemName, DeID);
			}
			else {
				Item->Item.Emplace(ItemName, this->GetNewID());
				this->UpdateMaxID();
			}

			return;
		}

		return;
	}
}


void UItemIdEditorManager::AddDeleteID(int32 ID) {

	if (!IsValid(this->GetIdDataTable()))
	{
		return;
	}

	for (const FName& RowName : ItemIdPtr->GetRowNames())
	{
		FItemId* Id = ItemIdPtr->FindRow<FItemId>(RowName, TEXT("AddDeleteID"));
		if (Id) {
			Id->DeleteID.Emplace(ID);
			UE_LOG(LogTemp, Warning, TEXT("被删除的ID[%d]，添加成功"), ID);
			return;
		}
	}
}

int32 UItemIdEditorManager::GetDeleteID() {
	if (!IsValid(this->GetIdDataTable()))
	{
		return -1;
	}

	for (const FName& RowName : ItemIdPtr->GetRowNames())
	{
		FItemId* Id = ItemIdPtr->FindRow<FItemId>(RowName, TEXT("GetDeleteID"));
		if (Id) {
			if (Id->DeleteID.Num())
			{
				return Id->DeleteID[0];
			}
		}

		return -1;
	}

	return -1;
}

void UItemIdEditorManager::RemoveDeleteID(int32 ID) {
	if (!IsValid(this->GetIdDataTable()))
	{
		return;
	}

	for (const FName& RowName : ItemIdPtr->GetRowNames())
	{
		FItemId* Id = ItemIdPtr->FindRow<FItemId>(RowName, TEXT("GetRemoveDeleteID"));
		if (Id) {
			Id->DeleteID.Remove(ID);
		}
	}
}

void UItemIdEditorManager::UpdateMaxID() {
	if (!IsValid(this->GetIdDataTable()))
	{
		return;
	}

	for (const FName& RowName : ItemIdPtr->GetRowNames())
	{
		FItemId* Id = ItemIdPtr->FindRow<FItemId>(RowName, TEXT("GetRemoveDeleteID"));
		if (Id) {
			Id->MaxID = Id->MaxID + 1;
		}

		return;
	}
}

void UItemIdEditorManager::GetRowNames(TArray<FString>& OutNames) {
	OutNames.Append({
	TEXT("小笼包"),
TEXT("冰冻小笼包"),
TEXT("贵族小笼包"),
TEXT("双层小笼包"),
TEXT("双层冰冻小笼包"),
TEXT("三向小笼包"),
TEXT("三向冰冻小笼包"),
TEXT("机枪小笼包"),
TEXT("机枪冰冻小笼包"),
TEXT("咖啡杯"),
TEXT("咖啡喷壶"),
TEXT("关东煮喷锅"),
TEXT("水上咖啡杯"),
TEXT("香肠"),
TEXT("热狗大炮"),
TEXT("双向水管"),
TEXT("控温双向水管"),
TEXT("合金水管"),
TEXT("三线酒架"),
TEXT("强力三线酒架"),
TEXT("终结者酒架"),
TEXT("炭烧海星"),
TEXT("芝士焗海星"),
TEXT("芥末海星刺身"),
TEXT("玉蜀黍"),
TEXT("奶油玉米机枪"),
TEXT("加农玉米机枪"),
TEXT("天秤座精灵"),
TEXT("天秤座战将"),
TEXT("天秤座星宿"),
TEXT("射手座精灵"),
TEXT("射手座战将"),
TEXT("射手座星宿"),
TEXT("弹簧虎"),
TEXT("飞行弹簧虎"),
TEXT("机器弹簧虎"),
TEXT("呆呆鸡"),
TEXT("阿瑞斯神使"),
TEXT("阿瑞斯圣神"),
TEXT("战神·阿瑞斯"),
TEXT("至尊战神"),
TEXT("丘比特神使"),
TEXT("丘比特圣神"),
TEXT("爱神·丘比特"),
TEXT("至尊爱神"),
TEXT("龙虾"),
TEXT("鲈鱼"),
TEXT("烧鸡"),
TEXT("巧克力投手"),
TEXT("臭豆腐投手"),
TEXT("色拉投手"),
TEXT("煮蛋器投手"),
TEXT("威力煮蛋器投手"),
TEXT("强袭煮蛋器投手"),
TEXT("冰冻煮蛋器投手"),
TEXT("雪糕投手"),
TEXT("麦旋风投手"),
TEXT("水果雪芭投手"),
TEXT("双鱼座精灵"),
TEXT("双鱼座战将"),
TEXT("双鱼座星宿"),
TEXT("弹弹鸡"),
TEXT("寒冰弹弹鸡"),
TEXT("月光弹弹鸡"),
TEXT("烤蜥蜴投手"),
TEXT("坚果蜥蜴投手"),
TEXT("花椒蜥蜴投手"),
TEXT("索尔神使"),
TEXT("索尔圣神"),
TEXT("雷神·索尔"),
TEXT("至尊雷神"),
TEXT("章鱼烧"),
TEXT("两栖章鱼烧"),
TEXT("火影章鱼烧"),
TEXT("巨蟹座精灵"),
TEXT("巨蟹座战将"),
TEXT("巨蟹座星宿"),
TEXT("糖葫芦炮弹"),
TEXT("水果糖葫芦弹"),
TEXT("七彩糖葫芦弹"),
TEXT("小火炉"),
TEXT("日光炉"),
TEXT("太阳能高效炉"),
TEXT("大火炉"),
TEXT("七周年蜡烛"),
TEXT("酒杯灯"),
TEXT("节能灯"),
TEXT("高效节能灯"),
TEXT("暖暖鸡"),
TEXT("焰羽暖暖鸡"),
TEXT("双子座精灵"),
TEXT("双子座战将"),
TEXT("双子座星宿"),
TEXT("阿波罗神使"),
TEXT("阿波罗圣神"),
TEXT("太阳神·阿波罗"),
TEXT("木盘子"),
TEXT("友情木盘子"),
TEXT("土司面包"),
TEXT("巧克力面包"),
TEXT("菠萝爆炸面包"),
TEXT("瓜皮护罩"),
TEXT("棉花糖"),
TEXT("处女座精灵"),
TEXT("赫拉神使"),
TEXT("赫拉圣神"),
TEXT("天后·赫拉"),
TEXT("苏打气泡"),
TEXT("五彩香皂泡泡"),
TEXT("汉堡包"),
TEXT("吞噬龙"),
TEXT("猫猫盒"),
TEXT("猫猫箱"),
TEXT("雷电长棍面包"),
TEXT("面粉袋"),
TEXT("木塞子"),
TEXT("钢丝球"),
TEXT("油灯"),
TEXT("换气扇"),
TEXT("咖啡粉"),
TEXT("酒瓶炸弹"),
TEXT("可乐炸弹"),
TEXT("爆竹"),
TEXT("10周年烟花"),
TEXT("冰桶炸弹"),
TEXT("开水壶炸弹"),
TEXT("老鼠夹子"),
TEXT("麻辣肉串炸弹"),
TEXT("辣椒粉"),
TEXT("幻幻鸡"),
TEXT("圣诞包裹"),
TEXT("猪猪加强器"),
TEXT("冰淇淋"),
TEXT("果蔬冰淇淋"),
TEXT("极寒冰沙"),
TEXT("樱桃反弹布丁"),
TEXT("节能反弹布丁"),
TEXT("布丁汪"),
TEXT("洛基神使"),
TEXT("洛基圣神"),
TEXT("火神·洛基"),
TEXT("至尊火神"),
TEXT("火盆"),
TEXT("电子烤盘"),
TEXT("岩烧烤盘"),
TEXT("金牛座精灵"),
TEXT("金牛座战将"),
TEXT("金牛座星宿"),
TEXT("鱼刺"),
TEXT("火龙果"),
TEXT("榴莲"),
TEXT("摩羯座精灵"),
TEXT("旋转咖啡喷壶"),
TEXT("节能旋转咖啡壶"),
TEXT("原子咖啡壶"),
TEXT("狮子座精灵"),
TEXT("狮子座战将"),
TEXT("狮子座星宿"),
TEXT("波塞冬神使"),
TEXT("波塞冬圣神"),
TEXT("海神·波塞冬"),
TEXT("洞君的工具袋"),
TEXT("阿诺德弹药包"),
TEXT("神殿背包"),
TEXT("冰渣的法师储物袋"),
TEXT("轰隆隆的时空行囊"),
TEXT("深渊背包"),
TEXT("美食初心者行囊"),
TEXT("美食冒险者行囊"),
TEXT("美食勇者行囊"),
TEXT("美食英雄行囊"),
TEXT("菜鸟饭盒"),
TEXT("异次元空间袋"),
TEXT("全防御卡礼盒"),
TEXT("全12防御卡礼盒"),
TEXT("全材料礼盒"),
TEXT("全装备礼盒"),
TEXT("龙虾礼盒"),
TEXT("幸运雷神礼盒"),
TEXT("幸运爱神礼盒"),
TEXT("神秘星座礼盒"),
TEXT("神秘鸡年纪念卡礼盒"),
TEXT("神秘狗年纪念卡礼盒"),
TEXT("神秘虎年纪念卡礼盒"),
TEXT("十三香礼包"),
TEXT("0.3补偿礼盒"),
TEXT("点券礼盒"),
TEXT("金币礼盒"),
TEXT("礼券礼盒"),
TEXT("威望礼盒"),
TEXT("0.5更新礼盒"),
TEXT("0.4补偿礼盒"),
TEXT("0.5延迟更新补偿礼盒"),
TEXT("巨蟹座一转礼盒"),
TEXT("巨蟹座二转礼盒"),
TEXT("双子座一转礼盒"),
TEXT("双子座二转礼盒"),
TEXT("双鱼座一转礼盒"),
TEXT("双鱼座二转礼盒"),
TEXT("射手座一转礼盒"),
TEXT("射手座二转礼盒"),
TEXT("巨蟹座技能书礼盒"),
TEXT("双子座技能书礼盒"),
TEXT("双鱼座技能书礼盒"),
TEXT("天秤座技能书礼盒"),
TEXT("天秤座一转礼盒"),
TEXT("天秤座二转礼盒"),
TEXT("金牛座一转礼盒"),
TEXT("金牛座二转礼盒"),
TEXT("狮子座一转礼盒"),
TEXT("狮子座二转礼盒"),
TEXT("0.6更新礼盒"),
TEXT("0.6补偿礼盒"),
TEXT("大点券礼盒"),
TEXT("2023新年礼盒"),
TEXT("强化礼盒"),
TEXT("武器礼盒"),
TEXT("0.7更新礼盒"),
TEXT("新人礼盒"),
TEXT("5级礼盒"),
TEXT("10级礼盒"),
TEXT("15级礼盒"),
TEXT("20级礼盒"),
TEXT("30级礼盒"),
TEXT("40级礼盒"),
TEXT("爆破专家6星卡牌夹"),
TEXT("爆破专家4星卡牌夹"),
TEXT("美味岛6星卡牌夹"),
TEXT("美味岛4星卡牌夹"),
TEXT("火山岛6星卡牌夹"),
TEXT("火山岛4星卡牌夹"),
TEXT("宝石强化礼盒"),
TEXT("100金币"),
TEXT("200金币"),
TEXT("2000金币"),
TEXT("5000金币"),
TEXT("50礼券"),
TEXT("100礼券"),
TEXT("100工资礼券"),
TEXT("200工资礼券"),
TEXT("300工资礼券"),
TEXT("400工资礼券"),
TEXT("500工资礼券"),
TEXT("10威望"),
TEXT("50威望"),
TEXT("100威望"),
TEXT("300威望"),
TEXT("神殿钥匙礼包"),
TEXT("深渊钥匙礼包"),
TEXT("小笼机枪"),
TEXT("星星枪"),
TEXT("钢爪枪"),
TEXT("双向水枪"),
TEXT("泡泡枪"),
TEXT("冰勺弩枪"),
TEXT("12周年章鱼炮"),
TEXT("海神镖枪"),
TEXT("宙斯神弩"),
TEXT("小圆饼盾牌"),
TEXT("榴弹炮"),
TEXT("激光宝石"),
TEXT("猫眼宝石"),
TEXT("冰冻宝石"),
TEXT("攻击宝石"),
TEXT("圣龙衣"),
TEXT("樱花风"),
TEXT("夏日沙滩_女"),
TEXT("棉花糖礼裙"),
TEXT("田园风情"),
TEXT("金色蝴蝶结礼服"),
TEXT("大小姐"),
TEXT("玛丽王后"),
TEXT("糖果心情"),
TEXT("海洋之恋"),
TEXT("pink宝贝"),
TEXT("爱丽丝"),
TEXT("棉花球"),
TEXT("霜降"),
TEXT("春丽"),
TEXT("小文静"),
TEXT("春日发型"),
TEXT("金色麻花辫"),
TEXT("浓情可可"),
TEXT("超萌兔娘"),
TEXT("兔尾巴"),
TEXT("红棕马尾"),
TEXT("俏皮妞"),
TEXT("黑色初音"),
TEXT("薰衣草浪漫"),
TEXT("可爱学生妹"),
TEXT("可爱古风"),
TEXT("粉红桃心镜"),
TEXT("心形发夹"),
TEXT("橙色花朵头饰"),
TEXT("金色短耳"),
TEXT("兔耳"),
TEXT("春之樱"),
TEXT("海星发夹"),
TEXT("pink宝贝帽"),
TEXT("田园蝴蝶发卡"),
TEXT("桃花朵朵"),
TEXT("海星星"),
TEXT("闹闹"),
TEXT("刚柔并济"),
TEXT("粉瞳"),
TEXT("玫瑰宝石"),
TEXT("七夕爱恋"),
TEXT("大眼妹"),
TEXT("明媚心情"),
TEXT("美宝石"),
TEXT("圣诞女生表情"),
TEXT("星星眼"),
TEXT("灿若桃花"),
TEXT("清澈之眸"),
TEXT("星空紫"),
TEXT("小草莓"),
TEXT("蓝凌之羽"),
TEXT("大蝴蝶结"),
TEXT("荧光羽翼"),
TEXT("蝴蝶之翼"),
TEXT("光速飞行器"),
TEXT("冰之翼"),
TEXT("小天使之翼"),
TEXT("飞鸟与鱼_男"),
TEXT("爱の绅士"),
TEXT("远古猎人"),
TEXT("锦鲤小哥哥"),
TEXT("森之精灵_男"),
TEXT("夏日萤火_男"),
TEXT("龙族传说_男"),
TEXT("飞鸟与鱼_女"),
TEXT("蜜梨"),
TEXT("锦鲤小姐姐"),
TEXT("毛绒萌兔"),
TEXT("定制礼服_女"),
TEXT("萌娘套装"),
TEXT("森之精灵_女"),
TEXT("双子座情人"),
TEXT("鬼姬式神"),
TEXT("金色誓言_女"),
TEXT("樱之恋曲_女"),
TEXT("提拉米苏"),
TEXT("梦幻露娜"),
TEXT("梦恋花语"),
TEXT("初音"),
TEXT("守护者_女"),
TEXT("欢天喜地"),
TEXT("夏日萤火_女"),
TEXT("爱蜜莉雅"),
TEXT("龙族传说_女"),
TEXT("十三香礼包"),
TEXT("夏日萤火_男_Text"),
TEXT("神秘礼盒_mail"),
TEXT("全防御卡礼盒_mail"),
TEXT("全12防御卡礼盒_mail"),
TEXT("全材料礼盒_mail"),
TEXT("全装备礼盒_mail"),
TEXT("0.3Bug补偿_mail"),
TEXT("点券礼盒_mail"),
TEXT("金币礼盒_mail"),
TEXT("威望礼盒_mail"),
TEXT("礼券礼盒_mail"),
TEXT("0.4Bug补偿_mail"),
TEXT("0.5更新_mail"),
TEXT("0.5延迟更新_mail"),
TEXT("0.6更新_mail"),
TEXT("0.6补偿_mail"),
TEXT("大点券礼盒_mail"),
TEXT("2023新年礼盒_mail"),
TEXT("强化礼盒_mail"),
TEXT("武器礼盒_mail"),
TEXT("0.7更新_mail"),
TEXT("宝石强化礼盒_mail"),
TEXT("新人礼盒_mail"),
TEXT("5级礼盒_mail"),
TEXT("10级礼盒_mail"),
TEXT("15级礼盒_mail"),
TEXT("20级礼盒_mail"),
TEXT("30级礼盒_mail"),
TEXT("40级礼盒_mail"),
TEXT("老鼠夹子配方"),
TEXT("双层小笼包配方"),
TEXT("冰冻小笼包配方"),
TEXT("汉堡包配方"),
TEXT("香肠配方"),
TEXT("钢丝球配方"),
TEXT("换气扇配方"),
TEXT("油灯配方"),
TEXT("酒瓶炸弹配方"),
TEXT("面粉袋配方"),
TEXT("开水壶炸弹配方"),
TEXT("木塞子配方"),
TEXT("火盆配方"),
TEXT("咖啡喷壶配方"),
TEXT("鱼刺配方"),
TEXT("瓜皮护罩配方"),
TEXT("三线酒架配方"),
TEXT("巧克力面包配方"),
TEXT("冰桶炸弹配方"),
TEXT("三向小笼包配方"),
TEXT("双层冰冻小笼包配方"),
TEXT("三向冰冻小笼包配方"),
TEXT("樱桃反弹布丁配方"),
TEXT("关东煮喷锅配方"),
TEXT("猫猫盒配方"),
TEXT("猫猫箱配方"),
TEXT("雷电长棍面包配方"),
TEXT("菠萝爆炸面包配方"),
TEXT("热狗大炮配方"),
TEXT("色拉投手配方"),
TEXT("臭豆腐投手配方"),
TEXT("巧克力投手配方"),
TEXT("煮蛋器投手配方"),
TEXT("双向水管配方"),
TEXT("冰淇淋配方"),
TEXT("麻辣肉串炸弹配方"),
TEXT("棉花糖配方"),
TEXT("糖葫芦炮弹配方"),
TEXT("苏打气泡配方"),
TEXT("炭烧海星配方"),
TEXT("烤蜥蜴投手配方"),
TEXT("辣椒粉配方"),
TEXT("雪糕投手配方"),
TEXT("汤勺"),
TEXT("煮蛋器"),
TEXT("生鸡蛋"),
TEXT("包装袋"),
TEXT("菠萝"),
TEXT("电鳗鱼肉"),
TEXT("假牙"),
TEXT("礼盒"),
TEXT("生菜"),
TEXT("番茄"),
TEXT("色拉酱"),
TEXT("餐叉"),
TEXT("筷子"),
TEXT("红豆腐"),
TEXT("臭豆腐"),
TEXT("啤酒"),
TEXT("铁丝"),
TEXT("灯座"),
TEXT("煤油"),
TEXT("砂纸"),
TEXT("扇片"),
TEXT("生蜥蜴"),
TEXT("孜然"),
TEXT("辣椒面"),
TEXT("食盐"),
TEXT("木炭"),
TEXT("海星"),
TEXT("奶酪"),
TEXT("木块"),
TEXT("鲜鱼"),
TEXT("袋子"),
TEXT("贪吃猫"),
TEXT("盒子"),
TEXT("鱼丸"),
TEXT("汤料"),
TEXT("铁锅"),
TEXT("面饼"),
TEXT("牛肉"),
TEXT("咖啡壶"),
TEXT("发动机"),
TEXT("白砂糖"),
TEXT("三通管"),
TEXT("蛋筒"),
TEXT("巧克力"),
TEXT("无烟煤"),
TEXT("冰块"),
TEXT("阀门"),
TEXT("钢管"),
TEXT("酒架"),
TEXT("酒瓶"),
TEXT("葡萄"),
TEXT("西瓜"),
TEXT("小刀"),
TEXT("果冻胶"),
TEXT("樱桃"),
TEXT("奶油"),
TEXT("冰包子"),
TEXT("小麦粉"),
TEXT("水壶"),
TEXT("小蒸笼"),
TEXT("烤炉"),
TEXT("火药"),
TEXT("包子"),
TEXT("冰糖"),
TEXT("山楂"),
TEXT("小苏打"),
TEXT("吸管"),
TEXT("纯净水"),
TEXT("肉丁"),
TEXT("花椒"),
TEXT("干辣椒"),
TEXT("打片机"),
TEXT("炼乳"),
TEXT("牛奶"),
TEXT("棉白糖"),
TEXT("煮蛋器投手[一转A]"),
TEXT("煮蛋器投手[一转B]"),
TEXT("煮蛋器投手[二转A]"),
TEXT("煮蛋器投手[二转B]"),
TEXT("煮蛋器投手[二转C]"),
TEXT("双鱼座[一转A]"),
TEXT("双鱼座[一转B]"),
TEXT("双鱼座[二转A]"),
TEXT("双鱼座[二转B]"),
TEXT("双鱼座[二转C]"),
TEXT("小火炉[一转A]"),
TEXT("小火炉[一转B]"),
TEXT("小火炉[二转A]"),
TEXT("小火炉[二转B]"),
TEXT("小火炉[二转C]"),
TEXT("烤蜥蜴投手[一转A]"),
TEXT("烤蜥蜴投手[一转B]"),
TEXT("烤蜥蜴投手[二转A]"),
TEXT("烤蜥蜴投手[二转B]"),
TEXT("烤蜥蜴投手[二转C]"),
TEXT("暖暖鸡[一转A]"),
TEXT("暖暖鸡[一转B]"),
TEXT("酒杯灯[一转A]"),
TEXT("酒杯灯[一转B]"),
TEXT("酒杯灯[二转A]"),
TEXT("酒杯灯[二转B]"),
TEXT("酒杯灯[二转C]"),
TEXT("木盘子[一转A]"),
TEXT("木盘子[一转B]"),
TEXT("三线酒架[一转A]"),
TEXT("三线酒架[一转B]"),
TEXT("三线酒架[二转A]"),
TEXT("三线酒架[二转B]"),
TEXT("三线酒架[二转C]"),
TEXT("双向水管[一转A]"),
TEXT("双向水管[一转B]"),
TEXT("双向水管[二转A]"),
TEXT("双向水管[二转B]"),
TEXT("双向水管[二转C]"),
TEXT("炭烧海星[一转A]"),
TEXT("炭烧海星[一转B]"),
TEXT("炭烧海星[二转A]"),
TEXT("炭烧海星[二转B]"),
TEXT("炭烧海星[二转C]"),
TEXT("章鱼烧[一转A]"),
TEXT("章鱼烧[一转B]"),
TEXT("章鱼烧[二转A]"),
TEXT("章鱼烧[二转B]"),
TEXT("章鱼烧[二转C]"),
TEXT("双子座[一转A]"),
TEXT("双子座[一转B]"),
TEXT("双子座[二转A]"),
TEXT("双子座[二转B]"),
TEXT("双子座[二转C]"),
TEXT("冰淇淋[一转A]"),
TEXT("冰淇淋[一转B]"),
TEXT("冰淇淋[二转A]"),
TEXT("冰淇淋[二转B]"),
TEXT("冰淇淋[二转C]"),
TEXT("射手座[一转A]"),
TEXT("射手座[一转B]"),
TEXT("射手座[二转A]"),
TEXT("射手座[二转B]"),
TEXT("射手座[二转C]"),
TEXT("巨蟹座[一转A]"),
TEXT("巨蟹座[一转B]"),
TEXT("巨蟹座[二转A]"),
TEXT("巨蟹座[二转B]"),
TEXT("巨蟹座[二转C]"),
TEXT("樱桃反弹布丁[一转A]"),
TEXT("樱桃反弹布丁[一转B]"),
TEXT("雷神[三转A]"),
TEXT("雷神[三转B]"),
TEXT("雷神[三转C]"),
TEXT("雷神[四转A]"),
TEXT("雷神[四转B]"),
TEXT("雷神[四转C]"),
TEXT("雷神[终转A]"),
TEXT("雷神[终转B]"),
TEXT("雷神[终转C]"),
TEXT("爱神[三转A]"),
TEXT("爱神[三转B]"),
TEXT("爱神[三转C]"),
TEXT("爱神[四转A]"),
TEXT("爱神[四转B]"),
TEXT("爱神[四转C]"),
TEXT("爱神[终转A]"),
TEXT("爱神[终转B]"),
TEXT("爱神[终转C]"),
TEXT("战神[三转A]"),
TEXT("战神[三转B]"),
TEXT("战神[三转C]"),
TEXT("战神[四转A]"),
TEXT("战神[四转B]"),
TEXT("战神[四转C]"),
TEXT("战神[终转A]"),
TEXT("战神[终转B]"),
TEXT("战神[终转C]"),
TEXT("太阳神[三转A]"),
TEXT("太阳神[三转B]"),
TEXT("太阳神[三转C]"),
TEXT("太阳神[四转A]"),
TEXT("太阳神[四转B]"),
TEXT("太阳神[四转C]"),
TEXT("火神[三转A]"),
TEXT("火神[三转B]"),
TEXT("火神[三转C]"),
TEXT("火神[四转A]"),
TEXT("火神[四转B]"),
TEXT("火神[四转C]"),
TEXT("火神[终转A]"),
TEXT("火神[终转B]"),
TEXT("火神[终转C]"),
TEXT("海神[三转A]"),
TEXT("海神[三转B]"),
TEXT("海神[三转C]"),
TEXT("海神[四转A]"),
TEXT("海神[四转B]"),
TEXT("海神[四转C]"),
TEXT("赫拉[三转A]"),
TEXT("赫拉[三转B]"),
TEXT("赫拉[三转C]"),
TEXT("赫拉[四转A]"),
TEXT("赫拉[四转B]"),
TEXT("赫拉[四转C]"),
TEXT("雷神进化凭证"),
TEXT("爱神进化凭证"),
TEXT("海神进化凭证"),
TEXT("火神进化凭证"),
TEXT("战神进化凭证"),
TEXT("太阳神进化凭证"),
TEXT("旋转咖啡喷壶[一转A]"),
TEXT("旋转咖啡喷壶[一转B]"),
TEXT("旋转咖啡喷壶[二转A]"),
TEXT("旋转咖啡喷壶[二转B]"),
TEXT("旋转咖啡喷壶[二转C]"),
TEXT("火盆[一转A]"),
TEXT("火盆[一转B]"),
TEXT("火盆[二转A]"),
TEXT("火盆[二转B]"),
TEXT("火盆[二转C]"),
TEXT("天秤座[一转A]"),
TEXT("天秤座[一转B]"),
TEXT("天秤座[二转A]"),
TEXT("天秤座[二转B]"),
TEXT("天秤座[二转C]"),
TEXT("狮子座[一转A]"),
TEXT("狮子座[一转B]"),
TEXT("狮子座[二转A]"),
TEXT("狮子座[二转B]"),
TEXT("狮子座[二转C]"),
TEXT("金牛座[一转A]"),
TEXT("金牛座[一转B]"),
TEXT("金牛座[二转A]"),
TEXT("金牛座[二转B]"),
TEXT("金牛座[二转C]"),
TEXT("弹弹鸡[一转A]"),
TEXT("弹弹鸡[一转B]"),
TEXT("弹弹鸡[二转A]"),
TEXT("弹弹鸡[二转B]"),
TEXT("弹弹鸡[二转C]"),
TEXT("弹簧虎[一转A]"),
TEXT("弹簧虎[一转B]"),
TEXT("弹簧虎[二转A]"),
TEXT("弹簧虎[二转B]"),
TEXT("弹簧虎[二转C]"),
TEXT("苏打气泡[一转A]"),
TEXT("苏打气泡[一转B]"),
TEXT("玉蜀黍[一转A]"),
TEXT("玉蜀黍[一转B]"),
TEXT("玉蜀黍[二转A]"),
TEXT("玉蜀黍[二转B]"),
TEXT("玉蜀黍[二转C]"),
TEXT("糖葫芦炮弹[一转A]"),
TEXT("糖葫芦炮弹[一转B]"),
TEXT("糖葫芦炮弹[二转A]"),
TEXT("糖葫芦炮弹[二转B]"),
TEXT("糖葫芦炮弹[二转C]"),
TEXT("天然香料"),
TEXT("上等香料"),
TEXT("秘制香料"),
TEXT("极品香料"),
TEXT("皇室香料"),
TEXT("魔幻香料"),
TEXT("精灵香料"),
TEXT("天使香料"),
TEXT("一级四叶草"),
TEXT("二级四叶草"),
TEXT("三级四叶草"),
TEXT("四级四叶草"),
TEXT("五级四叶草"),
TEXT("六级四叶草"),
TEXT("S级四叶草"),
TEXT("SS级四叶草"),
TEXT("SSS级四叶草"),
TEXT("SSR级四叶草"),
TEXT("小笼包初级技能书"),
TEXT("小笼包中级技能书"),
TEXT("小笼包高级技能书"),
TEXT("小笼包究级技能书"),
TEXT("吐司面包初级技能书"),
TEXT("吐司面包中级技能书"),
TEXT("吐司面包高级技能书"),
TEXT("吐司面包究级技能书"),
TEXT("巧克力面包初级技能书"),
TEXT("巧克力面包中级技能书"),
TEXT("巧克力面包高级技能书"),
TEXT("巧克力面包究级技能书"),
TEXT("瓜皮护罩初级技能书"),
TEXT("瓜皮护罩中级技能书"),
TEXT("瓜皮护罩高级技能书"),
TEXT("瓜皮护罩究级技能书"),
TEXT("棉花糖初级技能书"),
TEXT("棉花糖中级技能书"),
TEXT("棉花糖高级技能书"),
TEXT("棉花糖究级技能书"),
TEXT("关东煮喷锅初级技能书"),
TEXT("关东煮喷锅中级技能书"),
TEXT("关东煮喷锅高级技能书"),
TEXT("关东煮喷锅究级技能书"),
TEXT("鱼刺初级技能书"),
TEXT("鱼刺中级技能书"),
TEXT("鱼刺高级技能书"),
TEXT("鱼刺究级技能书"),
TEXT("木盘子初级技能书"),
TEXT("木盘子中级技能书"),
TEXT("木盘子高级技能书"),
TEXT("木盘子究级技能书"),
TEXT("巧克力投手初级技能书"),
TEXT("巧克力投手中级技能书"),
TEXT("巧克力投手高级技能书"),
TEXT("巧克力投手究级技能书"),
TEXT("机枪小笼包初级技能书"),
TEXT("机枪小笼包中级技能书"),
TEXT("机枪小笼包高级技能书"),
TEXT("机枪小笼包究级技能书"),
TEXT("冰冻煮蛋器投手初级技能书"),
TEXT("冰冻煮蛋器投手中级技能书"),
TEXT("冰冻煮蛋器投手高级技能书"),
TEXT("冰冻煮蛋器投手究级技能书"),
TEXT("小火炉初级技能书"),
TEXT("小火炉中级技能书"),
TEXT("小火炉高级技能书"),
TEXT("小火炉究级技能书"),
TEXT("战神初级技能书"),
TEXT("战神中级技能书"),
TEXT("战神高级技能书"),
TEXT("战神究级技能书"),
TEXT("三线酒架初级技能书"),
TEXT("三线酒架中级技能书"),
TEXT("三线酒架高级技能书"),
TEXT("三线酒架究级技能书"),
TEXT("酒杯灯初级技能书"),
TEXT("酒杯灯中级技能书"),
TEXT("酒杯灯高级技能书"),
TEXT("酒杯灯究级技能书"),
TEXT("太阳神初级技能书"),
TEXT("太阳神中级技能书"),
TEXT("太阳神高级技能书"),
TEXT("太阳神究级技能书"),
TEXT("玉蜀黍初级技能书"),
TEXT("玉蜀黍中级技能书"),
TEXT("玉蜀黍高级技能书"),
TEXT("玉蜀黍究级技能书"),
TEXT("炭烧海星初级技能书"),
TEXT("炭烧海星中级技能书"),
TEXT("炭烧海星高级技能书"),
TEXT("炭烧海星究级技能书"),
TEXT("章鱼烧初级技能书"),
TEXT("章鱼烧中级技能书"),
TEXT("章鱼烧高级技能书"),
TEXT("章鱼烧究级技能书"),
TEXT("雷神初级技能书"),
TEXT("雷神中级技能书"),
TEXT("雷神高级技能书"),
TEXT("雷神究级技能书"),
TEXT("大火炉初级技能书"),
TEXT("大火炉中级技能书"),
TEXT("大火炉高级技能书"),
TEXT("大火炉究级技能书"),
TEXT("冰冻小笼包初级技能书"),
TEXT("冰冻小笼包中级技能书"),
TEXT("冰冻小笼包高级技能书"),
TEXT("冰冻小笼包究级技能书"),
TEXT("双层小笼包初级技能书"),
TEXT("双层小笼包中级技能书"),
TEXT("双层小笼包高级技能书"),
TEXT("双层小笼包究级技能书"),
TEXT("咖啡杯初级技能书"),
TEXT("咖啡杯中级技能书"),
TEXT("咖啡杯高级技能书"),
TEXT("咖啡杯究级技能书"),
TEXT("咖啡喷壶初级技能书"),
TEXT("咖啡喷壶中级技能书"),
TEXT("咖啡喷壶高级技能书"),
TEXT("咖啡喷壶究级技能书"),
TEXT("双子座初级技能书"),
TEXT("双子座中级技能书"),
TEXT("双子座高级技能书"),
TEXT("双子座究级技能书"),
TEXT("暖暖鸡初级技能书"),
TEXT("暖暖鸡中级技能书"),
TEXT("暖暖鸡高级技能书"),
TEXT("暖暖鸡究级技能书"),
TEXT("双向水管初级技能书"),
TEXT("双向水管中级技能书"),
TEXT("双向水管高级技能书"),
TEXT("双向水管究级技能书"),
TEXT("煮蛋器投手初级技能书"),
TEXT("煮蛋器投手中级技能书"),
TEXT("煮蛋器投手高级技能书"),
TEXT("煮蛋器投手究级技能书"),
TEXT("爱神初级技能书"),
TEXT("爱神中级技能书"),
TEXT("爱神高级技能书"),
TEXT("爱神究级技能书"),
TEXT("双鱼座初级技能书"),
TEXT("双鱼座中级技能书"),
TEXT("双鱼座高级技能书"),
TEXT("双鱼座究级技能书"),
TEXT("天秤座初级技能书"),
TEXT("天秤座中级技能书"),
TEXT("天秤座高级技能书"),
TEXT("天秤座究级技能书"),
TEXT("巨蟹座初级技能书"),
TEXT("巨蟹座中级技能书"),
TEXT("巨蟹座高级技能书"),
TEXT("巨蟹座究级技能书"),
TEXT("机枪冰冻小笼包初级技能书"),
TEXT("机枪冰冻小笼包中级技能书"),
TEXT("机枪冰冻小笼包高级技能书"),
TEXT("机枪冰冻小笼包究级技能书"),
TEXT("三向小笼包初级技能书"),
TEXT("三向小笼包中级技能书"),
TEXT("三向小笼包高级技能书"),
TEXT("三向小笼包究级技能书"),
TEXT("三向冰冻小笼包初级技能书"),
TEXT("三向冰冻小笼包中级技能书"),
TEXT("三向冰冻小笼包高级技能书"),
TEXT("三向冰冻小笼包究级技能书"),
TEXT("弹弹鸡初级技能书"),
TEXT("弹弹鸡中级技能书"),
TEXT("弹弹鸡高级技能书"),
TEXT("弹弹鸡究级技能书"),
TEXT("海神初级技能书"),
TEXT("海神中级技能书"),
TEXT("海神高级技能书"),
TEXT("海神究级技能书"),
TEXT("火龙果初级技能书"),
TEXT("火龙果中级技能书"),
TEXT("火龙果高级技能书"),
TEXT("火龙果究级技能书"),
TEXT("榴莲初级技能书"),
TEXT("榴莲中级技能书"),
TEXT("榴莲高级技能书"),
TEXT("榴莲究级技能书"),
TEXT("呆呆鸡初级技能书"),
TEXT("呆呆鸡中级技能书"),
TEXT("呆呆鸡高级技能书"),
TEXT("呆呆鸡究级技能书"),
TEXT("赫拉神使初级技能书"),
TEXT("赫拉神使中级技能书"),
TEXT("赫拉神使高级技能书"),
TEXT("赫拉神使究级技能书"),
TEXT("雪糕投手初级技能书"),
TEXT("雪糕投手中级技能书"),
TEXT("雪糕投手高级技能书"),
TEXT("雪糕投手究级技能书"),
TEXT("生肖兑换券"),
TEXT("星座碎片"),
TEXT("神谕之石"),
TEXT("强化水晶"),
TEXT("高级强化水晶"),
TEXT("铁钻"),
TEXT("钢钻"),
TEXT("金刚钻"),
TEXT("月石钻"),
TEXT("拆卸改锥"),
TEXT("神殿钥匙"),
TEXT("神殿徽章A"),
TEXT("神殿徽章B"),
TEXT("深渊钥匙"),
TEXT("深渊徽章A"),
TEXT("深渊徽章B")
		});
}

/*

bool UItemIdEditorManager::AddDataTableRow(UDataTable* Table, FName RowName, const FTableRowBase& RowData)
{
	// We should never hit this! Stubbed to avoid NoExport on the class.
	check(0);
	return false;
}

bool UItemIdEditorManager::Generic_AddDataTableRow(UDataTable* Table, FName RowName, const uint8* RowData)
{
	return UItemIdEditorManager::AddNewRow_Source(Table, RowName, RowData);
}

DEFINE_FUNCTION(UItemIdEditorManager::execAddDataTableRow)
{
#if WITH_EDITOR
	P_GET_OBJECT(UDataTable, Table);
	P_GET_PROPERTY(FNameProperty, RowName);

	Stack.StepCompiledIn<FStructProperty>(NULL);
	void* RowDataPtr = Stack.MostRecentPropertyAddress;

	P_FINISH;
	bool bSuccess = false;

	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!Table)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("AddDataTableRow_MissingTableInput",
				"Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowDataPtr)
	{
		UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = Table->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) ||
			(OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			uint8* RowData = reinterpret_cast<uint8*>(RowDataPtr);
			bSuccess = Generic_AddDataTableRow(Table, RowName, RowData);
			P_NATIVE_END;
		}
		else
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				LOCTEXT("AddDataTableRow_IncompatibleProperty",
					"Incompatible output parameter; the data table's type is not the same as the return type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("AddDataTableRow_MissingOutputProperty",
				"Failed to resolve the output parameter for AddDataTableRow.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	*(bool*)RESULT_PARAM = bSuccess;
#endif

	* (bool*)RESULT_PARAM = false;
}

bool UItemIdEditorManager::AddNewRow_Source(UDataTable* Table, FName RowName, const uint8* NewRowData) {
#if WITH_EDITOR
	if (NewRowData && Table)
	{
		if ((RowName == NAME_None) || Table->GetRowMap().Contains(RowName) || !Table->GetRowStruct())
		{
			return false;
		}

		const FScopedTransaction Transaction(LOCTEXT("AddNewDataTableRow", "Add New Data Table Row"));
		Table->Modify();
		uint8* NewRow = FDataTableEditorUtils::AddRow(Table, RowName);
		check(NewRow);
		FDataTableEditorUtils::BroadcastPreChange(Table, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
		Table->RowStruct->CopyScriptStruct(NewRow, NewRowData);
		FDataTableEditorUtils::BroadcastPostChange(Table, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
		return true;
	}
#endif
	return false;
}*/