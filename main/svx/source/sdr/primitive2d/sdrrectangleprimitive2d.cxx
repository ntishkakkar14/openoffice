/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include "precompiled_svx.hxx"
#include <svx/sdr/primitive2d/sdrrectangleprimitive2d.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <svx/sdr/primitive2d/sdrdecompositiontools.hxx>
#include <drawinglayer/primitive2d/groupprimitive2d.hxx>
#include <svx/sdr/primitive2d/svx_primitivetypes2d.hxx>
#include <drawinglayer/primitive2d/sdrdecompositiontools2d.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace primitive2d
	{
		Primitive2DSequence SdrRectanglePrimitive2D::create2DDecomposition(const geometry::ViewInformation2D& /*aViewInformation*/) const
		{
			Primitive2DSequence aRetval;

			// create unit outline polygon
			const basegfx::B2DPolygon aUnitOutline(basegfx::tools::createPolygonFromRect(
                basegfx::B2DRange(0.0, 0.0, 1.0, 1.0), 
                getCornerRadiusX(), 
                getCornerRadiusY()));

			// add fill
			if(!getSdrLFSTAttribute().getFill().isDefault())
			{
				appendPrimitive2DReferenceToPrimitive2DSequence(aRetval, 
                    createPolyPolygonFillPrimitive(
                        basegfx::B2DPolyPolygon(aUnitOutline), 
                        getTransform(), 
                        getSdrLFSTAttribute().getFill(), 
                        getSdrLFSTAttribute().getFillFloatTransGradient()));
			}
            else if(getForceFillForHitTest())
            {
                // if no fill and it's a text frame, create a fill for HitTest and 
                // BoundRect fallback
                appendPrimitive2DReferenceToPrimitive2DSequence(aRetval,
                    createHiddenGeometryPrimitives2D(
                        true,
                        basegfx::B2DPolyPolygon(aUnitOutline),
                        getTransform()));
            }

			// add line
			if(!getSdrLFSTAttribute().getLine().isDefault())
			{
				appendPrimitive2DReferenceToPrimitive2DSequence(aRetval, 
                    createPolygonLinePrimitive(
                        aUnitOutline, 
                        getTransform(), 
                        getSdrLFSTAttribute().getLine(),
						attribute::SdrLineStartEndAttribute()));
			}
            else if(!getForceFillForHitTest())
            {
                // if initially no line is defined and it's not a text frame, create 
                // a line for HitTest and BoundRect
                appendPrimitive2DReferenceToPrimitive2DSequence(aRetval,
                    createHiddenGeometryPrimitives2D(
                        false,
                        basegfx::B2DPolyPolygon(aUnitOutline),
                        getTransform()));
            }

            // add text
			if(!getSdrLFSTAttribute().getText().isDefault())
			{
				appendPrimitive2DReferenceToPrimitive2DSequence(aRetval, 
                    createTextPrimitive(
                        basegfx::B2DPolyPolygon(aUnitOutline), 
                        getTransform(), 
                        getSdrLFSTAttribute().getText(), 
                        getSdrLFSTAttribute().getLine(), 
                        false, 
                        false, 
                        false));
			}

			// add shadow
			if(!getSdrLFSTAttribute().getShadow().isDefault())
			{
                aRetval = createEmbeddedShadowPrimitive(
                    aRetval, 
                    getSdrLFSTAttribute().getShadow());
			}

			return aRetval;
		}

		SdrRectanglePrimitive2D::SdrRectanglePrimitive2D(
			const basegfx::B2DHomMatrix& rTransform, 
			const attribute::SdrLineFillShadowTextAttribute& rSdrLFSTAttribute,
			double fCornerRadiusX, 
			double fCornerRadiusY,
            bool bForceFillForHitTest)
		:	BufferedDecompositionPrimitive2D(),
			maTransform(rTransform),
			maSdrLFSTAttribute(rSdrLFSTAttribute),
			mfCornerRadiusX(fCornerRadiusX),
			mfCornerRadiusY(fCornerRadiusY),
            mbForceFillForHitTest(bForceFillForHitTest)
		{
		}

		bool SdrRectanglePrimitive2D::operator==(const BasePrimitive2D& rPrimitive) const
		{
			if(BufferedDecompositionPrimitive2D::operator==(rPrimitive))
			{
				const SdrRectanglePrimitive2D& rCompare = (SdrRectanglePrimitive2D&)rPrimitive;

				return (getCornerRadiusX() == rCompare.getCornerRadiusX()
					&& getCornerRadiusY() == rCompare.getCornerRadiusY()
					&& getTransform() == rCompare.getTransform()
					&& getSdrLFSTAttribute() == rCompare.getSdrLFSTAttribute()
                    && getForceFillForHitTest() == rCompare.getForceFillForHitTest());
			}

			return false;
		}

		// provide unique ID
		ImplPrimitrive2DIDBlock(SdrRectanglePrimitive2D, PRIMITIVE2D_ID_SDRRECTANGLEPRIMITIVE2D)

	} // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
