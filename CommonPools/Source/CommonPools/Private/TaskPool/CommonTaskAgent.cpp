// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskPool/CommonTaskAgent.h"

ECommonStartTaskStatus UCommonTaskAgent::Start(UCommonTaskBase* InTask)
{
	if (InTask == nullptr)
	{
		return ECommonStartTaskStatus::Done;
	}

	Task = InTask;
	Task->SetTaskStatus(ECommonTaskStatus::Doing);
	return Task->Start();
}

void UCommonTaskAgent::Reset()
{
	Task = nullptr;
}
