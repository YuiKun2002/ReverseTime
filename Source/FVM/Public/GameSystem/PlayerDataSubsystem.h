// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameSystem/Tools/GameSystemFunction.h"
#include "PlayerDataSubsystem.generated.h"

/**
 * 角色存档子系统
 */

class USaveGame;
class UPlayerStructManager;
class ULocalPlayerDataHandle;


/**
 *数据接口
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UDataInterface : public UInterface
{
	GENERATED_BODY()
};

class FVM_API IDataInterface
{
	GENERATED_BODY()
public:

	/*

		Base

	*/

	//保存数据接口
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool SaveData(const FString& SaveLogMsg);
	virtual bool SaveData_Implementation(const FString& SaveLogMsg);

	//加载数据接口
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool LoadData(const FString& LoadLogMsg);
	virtual bool LoadData_Implementation(const FString& LoadLogMsg);

	//创建数据接口
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CreateData();
	virtual bool CreateData_Implementation();

	//移除数据接口
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool RemoveData();
	virtual bool RemoveData_Implementation();

	//获取存储Tag
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	const FString GetTag()const;
	virtual const FString GetTag_Implementation() const;

	//获取数据
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	USaveGame* GetData();
	virtual USaveGame* GetData_Implementation();

};

//本地数据句柄
UCLASS(BlueprintType)
class FVM_API ULocalDataHandle : public UObject, public IDataInterface
{
	GENERATED_BODY()

public:
	//保存本地存档
	virtual bool SaveData_Implementation(const FString& SaveLogMsg) override {
		this->SaveLocalSaveGame(this->SaveGameObject, SaveLogMsg);
		return true;
	};
	//加载本地存档
	virtual bool LoadData_Implementation(const FString& LoadLogMsg) override {
		this->SaveGameObject = this->LoadLocalSaveGame(LoadLogMsg);
		if (IsValid(this->SaveGameObject))
		{
			return true;
		}
		return false;
	};
	//创建新的数据
	virtual bool CreateData_Implementation() override {
		this->SaveGameObject = NewObject<USaveGame>(); return true;
	};
	//移除数据
	virtual bool RemoveData_Implementation() override {
		return UGameplayStatics::DeleteGameInSlot(this->GetLocalName(), 0);
	};
	//获取数据
	virtual USaveGame* GetData_Implementation() override { return this->SaveGameObject; };
	//获取存储Tag
	virtual const FString GetTag_Implementation() const override { return this->GetName(); };
public:

	UFUNCTION(BlueprintCallable)
	void CreateLocalName(const FString& Name);

	//获取本地名称+Tag
	UFUNCTION(BlueprintPure)
	const FString GetLocalName() const;

	//获取本地名称不加Tag
	UFUNCTION(BlueprintPure)
	const FString GetLocalNameBase() const;

	//检查本地文件是否存在
	UFUNCTION(BlueprintPure)
	bool LocalFileIsExist();

protected:
	//加载本地游戏存档
	USaveGame* LoadLocalSaveGame(const FString& LogMsg);
	//保存本地游戏存档
	void SaveLocalSaveGame(USaveGame* SaveGameObjectIns, const FString& LogMsg);
protected:
	//数据存储对象
	UPROPERTY()
	USaveGame* SaveGameObject = nullptr;
private:
	UPROPERTY()
	FString LocalName;
};

/*
角色数据子系统
*/
UCLASS()
class FVM_API UPlayerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//获取角色存档子系统
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统")
	static UPlayerDataSubsystem* GetPlayerDataSubsystemStatic();

	//--------------------------------------------------------------------------------------角色
	//创建一个角色空实例对象
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统 | 角色")
	UPlayerStructManager* CreateEmptyPlayerInstance();
	//获取角色数据实例
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统 | 角色")
	UPlayerStructManager* GetPlayerDataInstance();
	//设置一个新的角色实例对象
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统 | 角色")
	void SetPlayerDataInstance(UPlayerStructManager* NewPlayerIns);
	//设置一个新的本地角色数据句柄对象
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统 | 角色")
	void SetLocalPlayerDataHandle(ULocalPlayerDataHandle* NewPlayerHandle);
	//获取一个新的本地角色数据句柄对象
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统 | 角色")
	ULocalPlayerDataHandle* GetLocalPlayerDataHandle();
	//--------------------------------------------------------------------------------------数据

	//保存角色数据
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统 | 角色数据")
	bool SavePlayerData(UObject* PlayerDataInterface, const FString& SaveLogMsg = TEXT(""));
	//加载角色数据
	UFUNCTION(BlueprintCallable, Category = "角色存档子系统 | 角色数据")
	bool LoadPlayerData(UObject* PlayerDataInterface, const FString& LoadLogMsg = TEXT(""));

	//--------------------------------------------------------------------------------------本地
	//创建一个本地玩家存档【并且自动设置角色数据实例和本地角色句柄】
	UFUNCTION()
	bool CreateNewLocalPlayerData(ULocalPlayerDataHandle* LocalPlayerHandle);
	//通过名称保存一个本地玩家存档
	UFUNCTION()
	bool SaveLocalPlayerData(ULocalPlayerDataHandle* LocalPlayerHandle, const FString& SaveLogMsg);
	//通过名称直接加载一个本地玩家存档
	UFUNCTION()
	ULocalPlayerDataHandle* LoadLocalPlayerData(FString LocalPlayerName, const FString& LoadLogMsg);
	//通过名称删除一个本地玩家存档	
	UFUNCTION()
	bool RemoveLocalPlayerData(ULocalPlayerDataHandle* LocalPlayerHandle);
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------本地其他


	//--------------------------------------------------------------------------------------
private:
	//当前角色数据实例
	UPROPERTY()
	class UPlayerStructManager* CurrentPlayerDataInstance = nullptr;
	//本地角色对象
	UPROPERTY()
	ULocalPlayerDataHandle* CurrentLocalPlayerDataHandle = nullptr;
};
