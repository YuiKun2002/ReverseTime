// 该游戏是同人游戏，提供学习使用，禁止贩卖，如有侵权立刻删除


#include "GameSystem/MultiplayerOnlineObject.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameSystem/FVMGameInstance.h"
#include "GameSystem/PlayerStructManager.h"
#include "GameFramework/PlayerController.h"

UMultiplayerOnlineObject* UMultiplayerOnlineObject::MakeMultiplayerOnlineObject(
	UObject* WorldContextObject
) {
	UMultiplayerOnlineObject* NewMultiplayerOnlineObject = NewObject<UMultiplayerOnlineObject>(WorldContextObject);
	NewMultiplayerOnlineObject->WorldContextObject = WorldContextObject;
	NewMultiplayerOnlineObject->GameInstance = UFVMGameInstance::GetFVMGameInstance(WorldContextObject);
	return NewMultiplayerOnlineObject;
}

TSharedPtr<IOnlineSession> UMultiplayerOnlineObject::GetOnlineSession() {
	return Online::GetSubsystem(
		GEngine->GetWorldFromContextObject(WorldContextObject,
			EGetWorldErrorMode::ReturnNull), NAME_None
	)->GetSessionInterface();
}

UMultiplayerOnlineObject::UMultiplayerOnlineObject() :
	//绑定创建会话完成的委托
	OnCreateSessionCompleteDelegate(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerOnlineObject::OnCreateSessionComplete)
	),
	//绑定创建会话销毁的委托
	OnDestroySessionCompleteDelegate(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerOnlineObject::OnDestroySessionComplete)
	),
	//绑定查询会话的委托
	OnFindSessionsCompleteDelegate(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerOnlineObject::OnFindSessionsComplete)
	),
	//绑定加入会话的委托
	OnJoinSessionCompleteDelegate(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerOnlineObject::OnJoinSessionComplete)
	)
{

}

void UMultiplayerOnlineObject::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) {

	//清理句柄
	this->GetOnlineSession()->ClearOnCreateSessionCompleteDelegate_Handle(
		this->OnCreateSessionCompleteDelegateHd
	);

	//移除绑定
	if (bWasSuccessful)
	{
		//服务器登录成功
		if (this->ClientLoginComplete(true))
		{
			//打开关卡
			this->GetWorld()->ServerTravel("BP_GamePrepare_Level?listen", true);
		}
	}
}

void UMultiplayerOnlineObject::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) {

	this->GetOnlineSession()->ClearOnDestroySessionCompleteDelegate_Handle(
		this->OnDestroySessionCompleteDelegateHd
	);

	//默认会话
	if (SessionName.IsEqual(NAME_GameSession) && bWasSuccessful)
	{
		this->GameInstance->bMultiplayer = false;
		this->GameInstance->bCreateSession = false;
		this->GameInstance->LoginPlayerName.Empty(0);

		GAME_LOG_LINE(TEXT("销毁此会话:%s"), *(FName(NAME_GameSession).ToString()));
	}
}

void UMultiplayerOnlineObject::OnFindSessionsComplete(bool bWasSuccessful) {

	this->GetOnlineSession()->ClearOnFindSessionsCompleteDelegate_Handle(
		this->OnFindSessionsCompleteDelegateHd
	);

	TArray<FFindOnlineSearchResult> Results;
	if (bWasSuccessful && this->OnlineSessionSearchPtr.IsValid())
	{
		for (const auto& Result : this->OnlineSessionSearchPtr->SearchResults)
		{
			FFindOnlineSearchResult NewResult;
			Result.Session.SessionSettings.Get(GAME_SERVERCREATEROOMNAME, NewResult.RoomName);
			Result.Session.SessionSettings.Get(GAME_SERVERCREATEROOMPASSWORD, NewResult.RoomPassword);
			Result.Session.SessionSettings.Get(GAME_SERVERCREATEROOMPLAYERSGENDER, NewResult.RoomPlayersGender);
			NewResult.JoinRoomHandle = Result;
			Results.Emplace(NewResult);
		}
	}

	this->OnFindOnlineSearchDelegate.ExecuteIfBound(bWasSuccessful, Results);
}

void UMultiplayerOnlineObject::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Value) {
	this->GetOnlineSession()->ClearOnJoinSessionCompleteDelegate_Handle(
		this->OnJoinSessionCompleteDelegateHd
	);
	if (Value == EOnJoinSessionCompleteResult::Type::Success)
	{
		//客户端登录成功
		if (this->ClientLoginComplete(false))
		{
			APlayerController* Player = this->GetWorld()->GetFirstPlayerController();
			FString Ipv4;
			if (this->GetOnlineSession()->GetResolvedConnectString(NAME_GameSession, Ipv4) && IsValid(Player))
			{
				Player->ClientTravel(
					Ipv4, ETravelType::TRAVEL_Absolute
				);
			}
		}
	}
}

void UMultiplayerOnlineObject::CreateOnlineSession(FText CreateName, FText CreatePassword) {

	if (!this->GetOnlineSession().IsValid())
	{
		return;
	}

	//销毁之前的会话
	this->DestroyOnlineSession();

	//获取本地角色
	ULocalPlayer* LocalPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();

	//新的会话设置
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettings = MakeShareable<FOnlineSessionSettings>(
		new FOnlineSessionSettings()
	);

	//更新房间设置信息
	this->UpdateRoomInfor(OnlineSessionSettings.ToSharedRef(), CreateName, CreatePassword, {
	 GET_PLAYERDATAINS()->M_PlayerGender
		});

	//会话创建完成的委托
	this->OnCreateSessionCompleteDelegateHd =
		MoveTempIfPossible(
			this->GetOnlineSession()->AddOnCreateSessionCompleteDelegate_Handle(
				this->OnCreateSessionCompleteDelegate
			)
		);

	//创建会话
	this->GetOnlineSession()->CreateSession(
		*LocalPlayer->GetPreferredUniqueNetId(),
		NAME_GameSession,
		*OnlineSessionSettings
	);
}

bool UMultiplayerOnlineObject::DestroyOnlineSession() {

	if (!this->GetOnlineSession().IsValid())
	{
		return false;
	}

	if (this->ClientLoginFailed())
	{
		//如果已经创建了一个会话
		FNamedOnlineSession* OldOnlineSession = this->GetOnlineSession()->GetNamedSession(NAME_GameSession);
		if (OldOnlineSession)
		{
			//结束之前的会话
			this->OnDestroySessionCompleteDelegateHd =
				MoveTempIfPossible(
					this->GetOnlineSession()->AddOnDestroySessionCompleteDelegate_Handle(
						this->OnDestroySessionCompleteDelegate
					));
			this->GetOnlineSession()->DestroySession(OldOnlineSession->SessionName);

			return true;
		}
	}

	return false;
}

void UMultiplayerOnlineObject::FindOnlineSession(FOnFindOnlineSearchDelegate FindOnlineSearch) {

	if (!this->GetOnlineSession().IsValid())
	{
		FindOnlineSearch.ExecuteIfBound(false, TArray<FFindOnlineSearchResult>({}));
		return;
	}

	//只有客户端才能进行房间搜索
	if (!this->GameInstance->bMultiplayer && !this->GameInstance->bCreateSession)
	{
		//初始化搜索设置
		if (!this->OnlineSessionSearchPtr.IsValid())
		{
			this->OnlineSessionSearchPtr = MakeShareable<FOnlineSessionSearch>(new FOnlineSessionSearch());
			this->OnlineSessionSearchPtr->bIsLanQuery = true;
			this->OnlineSessionSearchPtr->MaxSearchResults = 20;
			this->OnlineSessionSearchPtr->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
			//this->OnlineSessionSearchPtr->QuerySettings.Set(TEXT("MatchType"), TEXT("FreeForAll"), EOnlineComparisonOp::Equals);
		}

		//获取本地角色
		ULocalPlayer* LocalPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();

		if (IsValid(LocalPlayer))
		{
			//查询委托
			if (this->OnFindSessionsCompleteDelegateHd.IsValid())
			{
				this->GetOnlineSession()->ClearOnFindSessionsCompleteDelegate_Handle(
					this->OnFindSessionsCompleteDelegateHd
				);
			}
			this->OnFindSessionsCompleteDelegateHd =
				MoveTempIfPossible(
					this->GetOnlineSession()->AddOnFindSessionsCompleteDelegate_Handle(
						this->OnFindSessionsCompleteDelegate
					));

			this->OnFindOnlineSearchDelegate.Clear();
			this->OnFindOnlineSearchDelegate = FindOnlineSearch;

			//开启搜索
			bool bFindSessions = this->GetOnlineSession()->FindSessions(
				*LocalPlayer->GetPreferredUniqueNetId(),
				this->OnlineSessionSearchPtr.ToSharedRef()
			);
			return;
		}
	}

	FindOnlineSearch.ExecuteIfBound(false, TArray<FFindOnlineSearchResult>({}));
}

void UMultiplayerOnlineObject::JoinOnlineSession(const FFindOnlineSearchResult& FindOnlineSearchResult) {
	if (this->GetOnlineSession().IsValid())
	{
		//获取本地角色
		ULocalPlayer* LocalPlayer = this->GetWorld()->GetFirstLocalPlayerFromController();
		if (IsValid(LocalPlayer))
		{
			this->DestroyOnlineSession();

			this->OnJoinSessionCompleteDelegateHd = MoveTempIfPossible(
				this->GetOnlineSession()->AddOnJoinSessionCompleteDelegate_Handle(
					this->OnJoinSessionCompleteDelegate
				));
			//玩家加入后，由服务器发送给客户端执行登陆成功的函数
			this->GetOnlineSession()->JoinSession(
				*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession,
				FindOnlineSearchResult.JoinRoomHandle
			);
		}
	}
}

void UMultiplayerOnlineObject::JoinOnlineSessionByIpv4(const FString& Ipv4) {
	if (this->GetOnlineSession().IsValid())
	{
		APlayerController* LocalPlayer = this->GetWorld()->GetFirstPlayerController<APlayerController>();
		if (IsValid(LocalPlayer))
		{
			APlayerController* Player = this->GetWorld()->GetFirstPlayerController();
			if (IsValid(Player))
			{
				Player->ClientTravel(Ipv4, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMultiplayerOnlineObject::UpdateOnlineSession(TArray<uint8> PlayersGender) {
	if (this->GetOnlineSession().IsValid() && this->GameInstance->bCreateSession)
	{
		//获取当前服务器的基本信息
		FOnlineSessionSettings* CurOnlineSessionSettings = this->GetOnlineSession()->GetSessionSettings(
			NAME_GameSession
		);
		FString RoomName, RoomPassword;
		if (CurOnlineSessionSettings)
		{

			CurOnlineSessionSettings->Get(GAME_SERVERCREATEROOMNAME, RoomName);
			CurOnlineSessionSettings->Get(GAME_SERVERCREATEROOMPASSWORD, RoomPassword);

			//更新会话设置
			TSharedPtr<FOnlineSessionSettings> NewOnlineSessionSettings = MakeShareable<FOnlineSessionSettings>(
				new FOnlineSessionSettings()
			);

			//更新房间设置信息
			this->UpdateRoomInfor(
				NewOnlineSessionSettings.ToSharedRef(),
				FText::FromString(RoomName), FText::FromString(RoomPassword), PlayersGender
			);
			this->GetOnlineSession()->UpdateSession(
				NAME_GameSession, *NewOnlineSessionSettings
			);
		}
	}
}

bool UMultiplayerOnlineObject::ClientLoginComplete(bool bServer) {
	if (IsValid(this->GameInstance))
	{
		UPlayerStructManager* LocalPlayerData = this->GameInstance->GetPlayerStructManager();
		if (IsValid(LocalPlayerData))
		{
			//设置玩家登陆名称
			this->GameInstance->LoginPlayerName = LocalPlayerData->M_PlayerName;
			this->GameInstance->bMultiplayer = true;
			this->GameInstance->bCreateSession = bServer;
			return true;
		}
	}
	return false;
}

bool UMultiplayerOnlineObject::ClientLoginFailed() {
	if (IsValid(this->GameInstance))
	{
		UPlayerStructManager* LocalPlayerData = this->GameInstance->GetPlayerStructManager();
		if (IsValid(LocalPlayerData))
		{
			//设置玩家登陆名称
			this->GameInstance->LoginPlayerName.Empty();
			this->GameInstance->bMultiplayer = false;
			this->GameInstance->bCreateSession = false;
			return true;
		}
	}
	return false;
}

bool UMultiplayerOnlineObject::ServerTravel(FString MapPath) {

	if (IsValid(this->GameInstance))
	{
		UWorld* World = GEngine->GetWorldFromContextObject(this->WorldContextObject,
			EGetWorldErrorMode::LogAndReturnNull);

		if (IsValid(World))
		{
			return World->ServerTravel(FString(MapPath), true);
		}
	}
	return false;
}

void UMultiplayerOnlineObject::GetOnlineSessionIpv4Address(FString& Ip, FString& Port) {
	Ip = TEXT("--");
	Port = TEXT("--");

	if (this->GetOnlineSession().IsValid())
	{
		FString I;
		if (this->GetOnlineSession()->GetResolvedConnectString(NAME_GameSession, I))
		{
			I.Split(TEXT(":"), &Ip, &Port);

			if (Port.Equals("0"))
			{
				Port = TEXT("--");
			}
		}
		else {
			//如果无法查询，则表示玩家通过IP地址加入的游戏
			Ip = TEXT("通过IP加入");
		}
	}
}

void UMultiplayerOnlineObject::GetOnlineSessionRoomName(FString& RoomName) {
	if (this->GetOnlineSession().IsValid() && this->GameInstance->bCreateSession)
	{
		FOnlineSessionSettings* CurOnlineSessionSettings = this->GetOnlineSession()->GetSessionSettings(
			NAME_GameSession
		);
		if (CurOnlineSessionSettings)
		{
			CurOnlineSessionSettings->Get(GAME_SERVERCREATEROOMNAME, RoomName);
		}
	}
}

void UMultiplayerOnlineObject::UpdateRoomInfor(
	TSharedRef<FOnlineSessionSettings> OnlineSetings,
	FText CreateName, FText CreatePassword, const TArray<uint8>& PlayerGender
) {
	//基础设置
	OnlineSetings->bIsLANMatch = false;
	OnlineSetings->NumPrivateConnections = 4;
	OnlineSetings->NumPublicConnections = 4;
	OnlineSetings->bAllowInvites = true;
	OnlineSetings->bAllowJoinInProgress = true;
	OnlineSetings->bShouldAdvertise = true;
	OnlineSetings->bAllowJoinViaPresence = true;
	OnlineSetings->bAllowJoinViaPresenceFriendsOnly = false;

	//新增会话设置-设置房间名称
	FOnlineSessionSetting RoomNameSetting;
	RoomNameSetting.Data = *(CreateName.ToString());
	RoomNameSetting.AdvertisementType = EOnlineDataAdvertisementType::Type::ViaOnlineService;
	//设置房间密码
	FOnlineSessionSetting RoomPasswordSetting;
	RoomPasswordSetting.Data = *(CreatePassword.ToString());
	RoomPasswordSetting.AdvertisementType = EOnlineDataAdvertisementType::Type::ViaOnlineService;
	//房间的玩家性别
	FOnlineSessionSetting RoomPlayerGenderSetting;
	RoomPlayerGenderSetting.Data.SetValue(PlayerGender);
	RoomPlayerGenderSetting.AdvertisementType = EOnlineDataAdvertisementType::Type::ViaOnlineService;

	//加入当前会话设置
	OnlineSetings->Set(GAME_SERVERCREATEROOMNAME, RoomNameSetting);
	OnlineSetings->Set(GAME_SERVERCREATEROOMPASSWORD, RoomPasswordSetting);
	OnlineSetings->Set(GAME_SERVERCREATEROOMPLAYERSGENDER, RoomPlayerGenderSetting);

	//OnlineSetings->Set(TEXT("MatchType"), TEXT("FreeForAll"),EOnlineDataAdvertisementType::ViaOnlineService);

}