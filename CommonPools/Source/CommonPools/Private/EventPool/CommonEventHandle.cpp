// Fill out your copyright notice in the Description page of Project Settings.


#include "EventPool/CommonEventHandle.h"

void FCommonEventUnifiedDelegate::Execute(UObject* Sender, UCommonEventArgsBase* EventArgs)
{
	switch (VariantDelegate.GetIndex())
	{
		case FCommonEventDelegateVariant::IndexOfType<FCommonEventDynamicDelegate>():
		{
			const FCommonEventDynamicDelegate& Delegate = VariantDelegate.Get<FCommonEventDynamicDelegate>();
			Delegate.ExecuteIfBound(Sender, EventArgs);
			break;
		}
		case FCommonEventDelegateVariant::IndexOfType<FCommonEventFunction>():
		{
			if (const FCommonEventFunction& Callback = VariantDelegate.Get<FCommonEventFunction>())
			{
				Callback(Sender, EventArgs);
			}
			break;
		}
		default: ;
	}
}

bool FCommonEventUnifiedDelegate::IsBound() const
{
	switch (VariantDelegate.GetIndex())
	{
		case FCommonEventDelegateVariant::IndexOfType<FCommonEventDynamicDelegate>():
			return VariantDelegate.Get<FCommonEventDynamicDelegate>().IsBound();
		case FCommonEventDelegateVariant::IndexOfType<FCommonEventFunction>():
			return VariantDelegate.Get<FCommonEventFunction>() != nullptr;
		default: return false;
	}
}

const void* FCommonEventUnifiedDelegate::GetBoundObject() const
{
	switch (VariantDelegate.GetIndex())
	{
		case FCommonEventDelegateVariant::IndexOfType<FCommonEventDynamicDelegate>():
		{
			const FCommonEventDynamicDelegate& Delegate = VariantDelegate.Get<FCommonEventDynamicDelegate>();
			if (Delegate.IsBound()) { return Delegate.GetUObject(); }
			break;
		}
		default: break;
	}
	return nullptr;
}

uint32 GetTypeHash(const FCommonEventHandle& Handle)
{
	return GetTypeHash(Handle.GetIndex());
}
