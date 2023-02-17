/** ###################################################################
**     Filename  : CurveHeader.h
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 25/01/2018, 14:28
**     Contents  :
**         User source code
**
** ###################################################################*/

#ifndef __CurveHeader_H
#define __CurveHeader_H

/* MODULE CurveHeader */


// public interface ---------------------------------------------------------
typedef enum
{
    eBattVoltCurve =0U,
    eResCurve,
    eResFuelCurve,
    eResLevel,
    eCapacitiveFuelSensor,
    eCapacitiveFuelVolt,
    eSecondryFuelTank,
    eNumofCurveTypes
} eCurveType;

void vGetCurveDataValue(eCurveType, u32, u32 *);
 
/* END CurveHeader */

#endif

