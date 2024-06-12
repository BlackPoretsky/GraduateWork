#pragma once

#include "CoreMinimal.h"

#include "AfterPenetrationAvoidanceFeeler.generated.h"

USTRUCT()
struct FAfterPenetrationAvoidanceFeeler
{
    GENERATED_BODY()

    /** FRotator описывающий отклонение от основного луча */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    FRotator AdjustmentRot;

    /** Ќасколько сильно этот датчик повли€ет на конечную позицию, если ударитс€ об объект мира */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    float WorldWeight;

    /** Ќасколько сильно этот датчик вли€ет на конечную позицию, если он попадает в пешку (установка значени€ 0 вообще не приведет к
     * столкновению с пешками) */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    float PawnWeight;

    /** —тепень чувствительности датчика*/
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    float Extent;

    /** ћинимальный интервал между кадрами трассировками с помощью этого датчика, если в последнем кадре ничего не было задето */
    UPROPERTY(EditAnywhere, Category = "Penetration Avoidance Feeler")
    int32 TraceInterval;

    /**  оличество кадров с момента использовани€ этого датчика */
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