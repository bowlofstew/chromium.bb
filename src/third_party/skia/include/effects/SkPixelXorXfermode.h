
/*
 * Copyright 2007 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkPixelXorXfermode_DEFINED
#define SkPixelXorXfermode_DEFINED

#include "SkXfermode.h"

/** SkPixelXorXfermode implements a simple pixel xor (op ^ src ^ dst).
    This transformation does not follow premultiplied conventions, therefore
    this proc *always* returns an opaque color (alpha == 255). Thus it is
    not really usefull for operating on blended colors.
*/
class SkPixelXorXfermode : public SkXfermode {
public:
    SkPixelXorXfermode(SkColor opColor) : fOpColor(opColor) {}

    // override from SkFlattenable
    virtual void flatten(SkFlattenableWriteBuffer&);

    SK_DECLARE_PUBLIC_FLATTENABLE_DESERIALIZATION_PROCS(SkPixelXorXfermode)

protected:
    // override from SkXfermode
    virtual SkPMColor xferColor(SkPMColor src, SkPMColor dst);

private:
    SkColor fOpColor;

    SkPixelXorXfermode(SkFlattenableReadBuffer& rb);

    typedef SkXfermode INHERITED;
};

#endif
