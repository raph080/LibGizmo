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

#include "GizmoTransformRotate.h"
#include "LibBase.h"

IGizmo *CreateRotateGizmo() { return new CGizmoTransformRotate; }

CGizmoTransformRotate::CGizmoTransformRotate() : CGizmoTransform()
{
    m_RotateType = ROTATE_NONE;
    m_RotateTypePredict = ROTATE_NONE;
    m_Ng2 = 0;
    m_AngleSnap = 0.f;
}

CGizmoTransformRotate::~CGizmoTransformRotate() {}

bool CGizmoTransformRotate::CheckRotatePlan(tvector3 &vNorm, float factor,
                                            const tvector3 &rayOrig,
                                            const tvector3 &rayDir, int id)
{
    tvector3 df, inters;
    m_Axis2 = vNorm;
    m_plan = vector4(m_pMatrix->GetTranslation(), vNorm);
    m_plan.RayInter(inters, rayOrig, rayDir);

    df = inters - m_pMatrix->GetTranslation();
    df /= GetScreenFactor();

    if (((df.Length() / factor) > 0.9f) && ((df.Length() / factor) < 1.1f))
    {
        m_svgMatrix = *m_pMatrix;

        m_LockVertex = df;
        m_LockVertex.Normalize();

        m_Vtx = m_LockVertex;
        m_Vty.Cross(m_LockVertex, vNorm);
        m_Vty.Normalize();
        m_Vtx *= factor;
        m_Vty *= factor;
        m_Vtz.Cross(m_Vtx, m_Vty);
        m_Ng2 = 0;
        if (id != -1)
            m_Axis = GetVector(id);

        m_OrigScale.Scaling(GetTransformedVector(0).Length(),
                            GetTransformedVector(1).Length(),
                            GetTransformedVector(2).Length());

        m_InvOrigScale.Inverse(m_OrigScale);

        return true;
    }
    return false;
}

bool CGizmoTransformRotate::GetOpType(ROTATETYPE &type, unsigned int x,
                                      unsigned int y)
{
    tvector3 rayOrigin, rayDir, axis;
    tvector3 dir = m_pMatrix->GetTranslation() - m_CamSrc;
    dir.Normalize();

    BuildRay(x, y, rayOrigin, rayDir);

    if (mMask & AXIS_TRACKBALL)
        if (CheckRotatePlan(dir, 1.0f, rayOrigin, rayDir, -1))
        {
            tmatrix mt = *m_pMatrix;
            mt.NoTrans();
            // mt.Inverse();
            // m_Axis = m_Axis2 = dir;
            m_Axis.TransformPoint(mt);
            /*
            m_Axis *=tvector3(GetTransformedVector(0).Length(),
            GetTransformedVector(1).Length(),
            GetTransformedVector(2).Length());
            */
            type = ROTATE_TWIN;
            return true;
        }

    // plan 1 : X/Z
    m_Axis = GetTransformedVector(0);
    if (mMask & AXIS_X)
        if (CheckRotatePlan(m_Axis, 1.0f, rayOrigin, rayDir, 0))
        {
            type = ROTATE_X;
            return true;
        }
    m_Axis = GetTransformedVector(1);
    if (mMask & AXIS_Y)
        if (CheckRotatePlan(m_Axis, 1.0f, rayOrigin, rayDir, 1))
        {
            type = ROTATE_Y;
            return true;
        }
    m_Axis = GetTransformedVector(2);
    if (mMask & AXIS_Z)
        if (CheckRotatePlan(m_Axis, 1.0f, rayOrigin, rayDir, 2))
        {
            type = ROTATE_Z;
            return true;
        }

    // m_Axis = GetTransformedVector(dir);
    if (mMask & AXIS_SCREEN)
        if (CheckRotatePlan(dir, 1.2f, rayOrigin, rayDir, -1))
        {
            tmatrix mt = *m_pMatrix;
            mt.NoTrans();
            mt.Inverse();
            m_Axis = m_Axis2 = dir;
            m_Axis.TransformPoint(mt);

            m_Axis *= tvector3(GetTransformedVector(0).Length(),
                               GetTransformedVector(1).Length(),
                               GetTransformedVector(2).Length());

            type = ROTATE_SCREEN;
            return true;
        }

    type = ROTATE_NONE;

    return false;
}

bool CGizmoTransformRotate::OnMouseDown(unsigned int x, unsigned int y)
{
    if (m_pMatrix)
    {
        return GetOpType(m_RotateType, x, y);
    }

    m_RotateType = ROTATE_NONE;
    return false;
}

void CGizmoTransformRotate::Rotate1Axe(const tvector3 &rayOrigin,
                                       const tvector3 &rayDir)
{
    tvector3 inters;
    m_plan = vector4(m_pMatrix->GetTranslation(), m_Axis2);
    m_plan.RayInter(inters, rayOrigin, rayDir);

    tvector3 df = inters - m_pMatrix->GetTranslation();

    df.Normalize();
    m_LockVertex2 = df;

    float acosng = df.Dot(m_LockVertex);
    if ((acosng < -0.99999f) || (acosng > 0.99999f))
        m_Ng2 = 0.f;
    else
        m_Ng2 = (float)acos(acosng);

    if (df.Dot(m_Vty) > 0)
        m_Ng2 = -m_Ng2;

    tmatrix mt, mt2;

    if (m_bUseSnap)
    {
        m_Ng2 *= (360.0f / ZPI);
        SnapIt(m_Ng2, m_AngleSnap);
        m_Ng2 /= (360.0f / ZPI);
    }

    mt.RotationAxis(m_Axis, m_Ng2);
    mt.Multiply(m_InvOrigScale);
    mt.Multiply(m_svgMatrix);
    mt2 = m_OrigScale;
    mt2.Multiply(mt);
    *m_pMatrix = mt2;

    if (m_Axis == tvector3::ZAxis)
    {
        if (mEditQT)
        {
            mEditQT->z = m_Ng2;
        }
    }
}

void CGizmoTransformRotate::OnMouseMove(unsigned int x, unsigned int y)
{
    tvector3 rayOrigin, rayDir, axis;

    BuildRay(x, y, rayOrigin, rayDir);

    if (m_RotateType != ROTATE_NONE)
    {
        if (m_RotateType == ROTATE_TWIN)
        {
            tvector3 inters;
            tvector3 dir = m_pMatrix->GetTranslation() - m_CamSrc;
            dir.Normalize();

            m_plan = vector4(m_pMatrix->GetTranslation(), dir);
            m_plan.RayInter(inters, rayOrigin, rayDir);

            tvector3 df = inters - m_pMatrix->GetTranslation();
            df /= GetScreenFactor();
            float lng1 = df.Length();
            if (lng1 >= 1.0f)
                lng1 = 0.9f;

            float height = (float)sin(acos(lng1));
            tvector3 m_CamRealUp, camRight;
            camRight.Cross(m_Axis, m_CamUp);
            m_CamRealUp.Cross(camRight, dir);

            tvector3 idt = height * dir;
            idt += df;

            idt = m_LockVertex - idt;

            tmatrix mt, mt2;

            mt.LookAtLH(tvector3(0, 0, 0), idt, m_CamRealUp);
            mt.Multiply(m_InvOrigScale);
            mt.Multiply(m_svgMatrix);
            mt2 = m_OrigScale;
            mt2.Multiply(mt);
            *m_pMatrix = mt2;
            /*
            if (mEditQT)
            {
            *mEditQT = tquaternion(mt2);
            }
            */
        }
        else
        {
            Rotate1Axe(rayOrigin, rayDir);
        }

        // if (mTransform) mTransform->Update();
    }
    else
    {
        // predict move
        if (m_pMatrix)
        {
            GetOpType(m_RotateTypePredict, x, y);
        }
    }
}

void CGizmoTransformRotate::OnMouseUp(unsigned int x, unsigned int y)
{
    m_RotateType = ROTATE_NONE;
}
/*
            char tmps[512];
            sprintf(tmps, "%5.2f %5.2f %5.2f %5.2f", plCam.x, plCam.y, plCam.z,
   plCam.w ); MessageBoxA(NULL, tmps, tmps, MB_OK);
            */
void CGizmoTransformRotate::Draw()
{
    if (m_pMatrix)
    {

        ComputeScreenFactor();

        tvector3 right, up, frnt, dir;

        tvector3 orig(m_pMatrix->GetTranslation());

        tvector3 plnorm(m_CamSrc - orig);

        plnorm.Normalize();

        tplane plCam = vector4(plnorm, 0);

        dir = orig - m_CamSrc;
        dir.Normalize();

        right.Cross(dir, GetTransformedVector(1));
        right.Normalize();

        up.Cross(dir, right);
        up.Normalize();

        right.Cross(dir, up);
        right.Normalize();

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

        // Twin
        if (mMask & AXIS_TRACKBALL)
        {
            tvector4 color(0.2f, 0.2f, 0.2f, 1.f);

            if (m_RotateTypePredict == ROTATE_TWIN)
                color = tvector4(CGizmoTransform::SELECTION_COLOR, 1);

            PrepareCircle(orig, right * GetScreenFactor(),
                          up * GetScreenFactor(), color);
        }

        // Screen
        if (mMask & AXIS_SCREEN)
        {
            tvector4 color(0.3f, 1, 1, 1.f);

            if (m_RotateTypePredict == ROTATE_SCREEN)
                color = tvector4(CGizmoTransform::SELECTION_COLOR, 1);

            PrepareCircle(orig, up * 1.2f * GetScreenFactor(),
                          right * 1.2f * GetScreenFactor(), color);
        }

        // X
        right.Cross(dir, axeX);
        right.Normalize();
        frnt.Cross(right, axeX);

        frnt.Normalize();

        if (mMask & AXIS_X)
        {
            tvector4 color = tvector4(CGizmoTransform::X_AXIS_COLOR, 1);

            if (m_RotateTypePredict == ROTATE_X)
                color = tvector4(CGizmoTransform::SELECTION_COLOR, 1);

            PrepareCircleHalf(orig, right * GetScreenFactor(),
                              frnt * GetScreenFactor(), plCam, color);
        }

        // Y

        right.Cross(dir, axeY);
        right.Normalize();
        frnt.Cross(right, axeY);

        frnt.Normalize();

        if (mMask & AXIS_Y)
        {
            tvector4 color = tvector4(CGizmoTransform::Y_AXIS_COLOR, 1);

            if (m_RotateTypePredict == ROTATE_Y)
                color = tvector4(CGizmoTransform::SELECTION_COLOR, 1);

            PrepareCircleHalf(orig, right * GetScreenFactor(),
                              frnt * GetScreenFactor(), plCam, color);
        }

        // Z
        right.Cross(dir, axeZ);
        right.Normalize();
        frnt.Cross(right, axeZ);

        frnt.Normalize();

        if (mMask & AXIS_Z)
        {
            tvector4 color = tvector4(CGizmoTransform::Z_AXIS_COLOR, 1);

            if (m_RotateTypePredict == ROTATE_Z)
                color = tvector4(CGizmoTransform::SELECTION_COLOR, 1);

            PrepareCircleHalf(orig, right * GetScreenFactor(),
                              frnt * GetScreenFactor(), plCam, color);
        }
        // camembert
        if ((m_RotateType != ROTATE_NONE) && (m_RotateType != ROTATE_TWIN))
        {
            tvector4 innerCol = tvector4(CGizmoTransform::SELECTION_COLOR, .5f);
            tvector4 borderCol = tvector4(CGizmoTransform::SELECTION_COLOR, 1);
            PrepareCamem(orig, m_Vtx * GetScreenFactor(),
                         m_Vty * GetScreenFactor(), -m_Ng2, innerCol,
                         borderCol);
        }
    }
    CGizmoTransformRender::Draw();
}
