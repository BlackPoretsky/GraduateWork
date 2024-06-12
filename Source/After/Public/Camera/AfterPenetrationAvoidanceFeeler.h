#pragma once

#include "CoreMinimal.h"

#include "AfterPenetrationAvoidanceFeeler.generated.h"

USTRUCT()
struct FAfterPenetrationAvoidanceFeeler
{
    GENERATED_BODY()

    /** FRotator ����������� ���������� �� ��������� ���� */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    FRotator AdjustmentRot;

    /** ��������� ������ ���� ������ �������� �� �������� �������, ���� �������� �� ������ ���� */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    float WorldWeight;

    /** ��������� ������ ���� ������ ������ �� �������� �������, ���� �� �������� � ����� (��������� �������� 0 ������ �� �������� �
     * ������������ � �������) */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    float PawnWeight;

    /** ������� ���������������� �������*/
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    float Extent;

    /** ����������� �������� ����� ������� ������������� � ������� ����� �������, ���� � ��������� ����� ������ �� ���� ������ */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    int32 TraceInterval;

    /** ���������� ������ � ������� ������������� ����� ������� */
    UPROPERTY(Transient)
    int32 FramesUntilNextTrace;

    FAfterPenetrationAvoidanceFeeler()
        : AdjustmentRot(ForceInit),  //
          WorldWeight(0),            //
          PawnWeight(0),             //
          Extent(0),                 //
          TraceInterval(0),          //
          FramesUntilNextTrace(0)
    {
    }

    FAfterPenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot, const float& InWorldWeight, const float& InPawnWeight,
        const float& InExtent, const int32& InTraceInterval = 0,
        const int32& InFramesUntilNextTrace = 0)
        : AdjustmentRot(InAdjustmentRot),  //
          WorldWeight(InWorldWeight),      //
          PawnWeight(InPawnWeight),        //
          Extent(InExtent),                //
          TraceInterval(InTraceInterval),  //
          FramesUntilNextTrace(InFramesUntilNextTrace)
    {
    }
};