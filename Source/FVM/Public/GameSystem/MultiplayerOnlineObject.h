// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemUtils.h"
#include "UObject/NoExportTypes.h"
#include "OnlineSessionSettings.h"
#include "Templates/SharedPointer.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerOnlineObject.generated.h"

//服务器房间名称
#define GAME_SERVERCREATEROOMNAME TEXT("Server_RoomName")
//服务器房间密码
#define GAME_SERVERCREATEROOMPASSWORD TEXT("Server_RoomPassword")
//服务器角色的性别
#define GAME_SERVERCREATEROOMPLAYERSGENDER TEXT("Server_RoomPlayersGender")

//搜索结果
USTRUCT(BlueprintType)
struct FFindOnlineSearchResult {

	GENERATED_USTRUCT_BODY()

	//房间名称
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString RoomName;

	//房间密码
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString RoomPassword;

	//房间玩家性别
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<uint8> RoomPlayersGender;

	//登陆房间的句柄【蓝图不可见】
	FOnlineSessionSearchResult JoinRoomHandle;
};


DECLARE_DYNAMIC_DELEGATE_TwoParams(
	FOnFindOnlineSearchDelegate,
	bool, bWasSuccessful,
	const TArray<FFindOnlineSearchResult>&, Rooms
);
/**
 * 多人在线对象
 */
UCLASS(Blueprintable)
class FVM_API UMultiplayerOnlineObject : public UObject
{
	GENERATED_BODY()

public:

	UMultiplayerOnlineObject();

	//生成多人在线对象
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UMultiplayerOnlineObject* MakeMultiplayerOnlineObject(
		class UObject* WorldContextObject
	);

	//创建在线会话
	UFUNCTION(BlueprintCallable)
	void CreateOnlineSession(FText CreateName, FText CreatePassword);
	//销毁会话
	UFUNCTION(BlueprintCallable)
	bool DestroyOnlineSession();
	//查询会话
	UFUNCTION(BlueprintCallable)
	void FindOnlineSession(FOnFindOnlineSearchDelegate FindOnlineSearch);
	//加入会话
	UFUNCTION(BlueprintCallable)
	void JoinOnlineSession(const FFindOnlineSearchResult& FindOnlineSearchResult);
	//加入会话
	UFUNCTION(BlueprintCallable)
	void JoinOnlineSessionByIpv4(const FString& Ipv4);
	//更新会话 
	UFUNCTION(BlueprintCallable)
	void UpdateOnlineSession(TArray<uint8> PlayersGender);
	//客户端登陆成功
	UFUNCTION(BlueprintCallable)
	bool ClientLoginComplete(bool bServer);
	//客户端登陆失败
	UFUNCTION(BlueprintCallable)
	bool ClientLoginFailed();

	//无缝切换
	UFUNCTION(BlueprintCallable)
	bool ServerTravel(FString MapPath);

	//获取房间的Ipv4地址
	UFUNCTION(BlueprintPure)
	void GetOnlineSessionIpv4Address(FString& Ip,FString& Port);
	//获取房间的名称[服务器获取]
	UFUNCTION(BlueprintPure)
	void GetOnlineSessionRoomName(FString& RoomName);

protected:
	/*
		多人游戏
	*/

	TSharedPtr<IOnlineSession> GetOnlineSession();

	//当会话创建完成时
	FDelegateHandle OnCreateSessionCompleteDelegateHd;
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	//当销毁会话时
	FDelegateHandle OnDestroySessionCompleteDelegateHd;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	//查询会话时
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearchPtr;
	FDelegateHandle OnFindSessionsCompleteDelegateHd;
	FOnFindOnlineSearchDelegate OnFindOnlineSearchDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	void OnFindSessionsComplete(bool bWasSuccessful);

	//加入会话时
	FDelegateHandle OnJoinSessionCompleteDelegateHd;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Value);

	//更新房间信息
	void UpdateRoomInfor(
		TSharedRef<FOnlineSessionSettings> OnlineSetings,
		FText CreateName,
		FText CreatePassword,
		const TArray<uint8>& PlayerGender
	);

protected:

	UPROPERTY()
	class UFVMGameInstance* GameInstance = nullptr;

	UPROPERTY()
	UObject* WorldContextObject = nullptr;
};
