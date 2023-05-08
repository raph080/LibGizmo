///////////////////////////////////////////////////////////////////////////////////////////////////
// LibGizmo
// File Name :
// Creation : 10/01/2012
// Author : Cedric Guillemet
// Description : LibGizmo
//
/// Copyright (C) 2012 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do
/// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "GizmoTransformMove.h"
#include "LibBase.h"

IGizmo *CreateMoveGizmo() { return new CGizmoTransformMove; }

CGizmoTransformMove::CGizmoTransformMove() : CGizmoTransform()
{
    m_MoveType = MOVE_NONE;
}

CGizmoTransformMove::~CGizmoTransformMove() {}

bool CGizmoTransformMove::GetOpType(MOVETYPE &type, unsigned int x,
                                    unsigned int y)
{
    tvector3 rayOrigin, rayDir, df;
    BuildRay(x, y, rayOrigin, rayDir);

    m_svgMatrix = *m_pMatrix;

    tvector3 trss(GetTransformedVector(0).Length(),
                  GetTransformedVector(1).Length(),
                  GetTransformedVector(2).Length());

    tmatrix mt;
    if (mLocation == LOCATE_LOCAL)
    {
        mt = *m_pMatrix;
        mt.Inverse();
    }
    else
    {
        // world
        mt.Translation(-m_pMatrix->V4.position);
    }

    // plan 1 : X/Z
    df = RayTrace2(rayOrigin, rayDir, GetTransformedVector(1), mt, trss, false);

    if ((df.x >= 0) && (df.x <= 1) && (fabs(df.z) < 0.1f))
    {
        type = MOVE_X;
        return true;
    }
    else if ((df.z >= 0) && (df.z <= 1) && (fabs(df.x) < 0.1f))
    {
        type = MOVE_Z;
        return true;
    }
    else if ((df.x < 0.6f) && (df.z < 0.6f) && (df.x > 0.5f) && (df.z > 0.5f))
    {
        type = MOVE_XZ;
        return true;
    }
    else
    {

        // plan 2 : X/Y
        df = RayTrace2(rayOrigin, rayDir, GetTransformedVector(2), mt, trss,
                       false);

        if ((df.x >= 0) && (df.x <= 1) && (fabs(df.y) < 0.1f))
        {
            type = MOVE_X;
            return true;
        }
        if ((df.y >= 0) && (df.y <= 1) && (fabs(df.x) < 0.1f))
        {
            type = MOVE_Y;
            return true;
        }
        else if ((df.x < 0.6f) && (df.y < 0.6f) && (df.x > 0.5f) &&
                 (df.y > 0.5f))
        {
            type = MOVE_XY;
            return true;
        }
        else
        {
            // plan 3: Y/Z
            df = RayTrace2(rayOrigin, rayDir, GetTransformedVector(0), mt, trss,
                           false);

            if ((df.y >= 0) && (df.y <= 1) && (fabs(df.z) < 0.1f))
            {
                type = MOVE_Y;
                return true;
            }
            else if ((df.z >= 0) && (df.z <= 1) && (fabs(df.y) < 0.1f))
            {
                type = MOVE_Z;
                return true;
            }
            else if ((df.y < 0.6f) && (df.z < 0.6f) && (df.y > 0.5f) &&
                     (df.z > 0.5f))
            {
                type = MOVE_YZ;
                return true;
            }
        }
    }

    type = MOVE_NONE;
    return false;
}

bool CGizmoTransformMove::OnMouseDown(unsigned int x, unsigned int y)
{
    if (m_pMatrix)
    {
        return GetOpType(m_MoveType, x, y);
    }

    m_MoveType = MOVE_NONE;
    return false;
}

void CGizmoTransformMove::OnMouseMove(unsigned int x, unsigned int y)
{
    if (m_MoveType != MOVE_NONE)
    {
        tvector3 rayOrigin, rayDir, df, inters;

        BuildRay(x, y, rayOrigin, rayDir);
        m_plan.RayInter(inters, rayOrigin, rayDir);

        tvector3 axeX(1, 0, 0), axeY(0, 1, 0), axeZ(0, 0, 1);

        if (mLocation == LOCATE_LOCAL)
        {
            axeX.TransformVector(*m_pMatrix);
            axeY.TransformVector(*m_pMatrix);
            axeZ.TransformVector(*m_pMatrix);
            axeX.Normalize();
            axeY.Normalize();
            axeZ.Normalize();
        }

        df = inters - m_LockVertex;

        switch (m_MoveType)
        {
        case MOVE_XZ:
            df = tvector3(df.Dot(axeX), 0, df.Dot(axeZ));
            break;
        case MOVE_X:
            df = tvector3(df.Dot(axeX), 0, 0);
            break;
        case MOVE_Z:
            df = tvector3(0, 0, df.Dot(axeZ));
            break;
        case MOVE_XY:
            df = tvector3(df.Dot(axeX), df.Dot(axeY), 0);
            break;
        case MOVE_YZ:
            df = tvector3(0, df.Dot(axeY), df.Dot(axeZ));
            break;
        case MOVE_Y:
            df = tvector3(0, df.Dot(axeY), 0);
            break;
        }

        tvector3 adf;

        tmatrix mt;
        if (m_bUseSnap)
        {
            SnapIt(df.x, m_MoveSnap.x);
            SnapIt(df.y, m_MoveSnap.y);
            SnapIt(df.z, m_MoveSnap.z);
        }

        adf = df.x * axeX + df.y * axeY + df.z * axeZ;

        mt.Translation(adf);
        *m_pMatrix = m_svgMatrix;
        m_pMatrix->Multiply(mt);
        // if (mTransform) mTransform->Update();

        if (mEditPos)
            *mEditPos = m_pMatrix->V4.position;
    }
    else
    {
        // predict move
        if (m_pMatrix)
        {
            GetOpType(m_MoveTypePredict, x, y);
        }
    }
}

void CGizmoTransformMove::OnMouseUp(unsigned int x, unsigned int y)
{
    m_MoveType = MOVE_NONE;
}

void CGizmoTransformMove::Draw()
{
    ComputeScreenFactor();

    if (m_pMatrix)
    {
        // glDisable(GL_DEPTH_TEST);
        tvector3 orig = m_pMatrix->GetTranslation();

        tvector3 axeX(1, 0, 0), axeY(0, 1, 0), axeZ(0, 0, 1);

        if (mLocation == LOCATE_LOCAL)
        {
            axeX.TransformVector(*m_pMatrix);
            axeY.TransformVector(*m_pMatrix);
            axeZ.TransformVector(*m_pMatrix);
            axeX.Normalize();
            axeY.Normalize();
            axeZ.Normalize();
        }

        float quadThresh = 0.5f * GetScreenFactor();
        float quadSize = 0.1f * GetScreenFactor();

        tvector3 origQuadXZ = orig + (axeX + axeZ) * quadThresh;
        tvector3 origQuadXY = orig + (axeX + axeY) * quadThresh;
        tvector3 origQuadYZ = orig + (axeY + axeZ) * quadThresh;

        tvector4 innerColXZ = vector4(CGizmoTransform::Y_AXIS_COLOR, .5f);
        tvector4 innerColXY = vector4(CGizmoTransform::Z_AXIS_COLOR, .5f);
        tvector4 innerColYZ = vector4(CGizmoTransform::X_AXIS_COLOR, .5f);

        tvector4 borderColXZ = vector4(CGizmoTransform::Y_AXIS_COLOR, 1);
        tvector4 borderColXY = vector4(CGizmoTransform::Z_AXIS_COLOR, 1);
        tvector4 borderColYZ = vector4(CGizmoTransform::X_AXIS_COLOR, 1);

        if (m_MoveTypePredict == MOVE_XZ)
        {
            innerColXZ = vector4(CGizmoTransform::SELECTION_COLOR, .5f);
            borderColXZ = vector4(CGizmoTransform::SELECTION_COLOR, 1);
        }
        if (m_MoveTypePredict == MOVE_XY)
        {
            innerColXY = vector4(CGizmoTransform::SELECTION_COLOR, .5f);
            borderColXY = vector4(CGizmoTransform::SELECTION_COLOR, 1);
        }
        if (m_MoveTypePredict == MOVE_YZ)
        {
            innerColYZ = vector4(CGizmoTransform::SELECTION_COLOR, .5f);
            borderColYZ = vector4(CGizmoTransform::SELECTION_COLOR, 1);
        }

        // draw quads
        PrepareQuad(origQuadXZ, quadSize, axeX, axeZ, innerColXZ, borderColXZ);
        PrepareQuad(origQuadXY, quadSize, axeX, axeY, innerColXY, borderColXY);
        PrepareQuad(origQuadYZ, quadSize, axeY, axeZ, innerColYZ, borderColYZ);

        axeX *= GetScreenFactor();
        axeY *= GetScreenFactor();
        axeZ *= GetScreenFactor();

        tvector4 colX = vector4(CGizmoTransform::Y_AXIS_COLOR, 1);
        tvector4 colY = vector4(CGizmoTransform::Z_AXIS_COLOR, 1);
        tvector4 colZ = vector4(CGizmoTransform::X_AXIS_COLOR, 1);

        if (m_MoveTypePredict == MOVE_X)
            colX = vector4(CGizmoTransform::SELECTION_COLOR, 1);
        if (m_MoveTypePredict == MOVE_Y)
            colY = vector4(CGizmoTransform::SELECTION_COLOR, 1);
        if (m_MoveTypePredict == MOVE_Z)
            colZ = vector4(CGizmoTransform::SELECTION_COLOR, 1);

        // draw axis
        PrepareConeAxis(orig, axeX, axeY, axeZ, 0.05f, 0.83f, colX);
        PrepareConeAxis(orig, axeY, axeX, axeZ, 0.05f, 0.83f, colY);
        PrepareConeAxis(orig, axeZ, axeX, axeY, 0.05f, 0.83f, colZ);
    }

    CGizmoTransformRender::Draw();
}
