/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class aiMatrix3x3 : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal aiMatrix3x3(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(aiMatrix3x3 obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~aiMatrix3x3() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          AssimpPINVOKE.delete_aiMatrix3x3(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public aiMatrix3x3() : this(AssimpPINVOKE.new_aiMatrix3x3__SWIG_0(), true) {
  }

  public aiMatrix3x3(float _a1, float _a2, float _a3, float _b1, float _b2, float _b3, float _c1, float _c2, float _c3) : this(AssimpPINVOKE.new_aiMatrix3x3__SWIG_1(_a1, _a2, _a3, _b1, _b2, _b3, _c1, _c2, _c3), true) {
  }

  public aiMatrix3x3(aiMatrix4x4 pMatrix) : this(AssimpPINVOKE.new_aiMatrix3x3__SWIG_2(aiMatrix4x4.getCPtr(pMatrix)), true) {
    if (AssimpPINVOKE.SWIGPendingException.Pending) throw AssimpPINVOKE.SWIGPendingException.Retrieve();
  }

  public aiMatrix3x3 Transpose() {
    aiMatrix3x3 ret = new aiMatrix3x3(AssimpPINVOKE.aiMatrix3x3_Transpose(swigCPtr), false);
    return ret;
  }

  public aiMatrix3x3 Inverse() {
    aiMatrix3x3 ret = new aiMatrix3x3(AssimpPINVOKE.aiMatrix3x3_Inverse(swigCPtr), false);
    return ret;
  }

  public float Determinant() {
    float ret = AssimpPINVOKE.aiMatrix3x3_Determinant(swigCPtr);
    return ret;
  }

  public static aiMatrix3x3 RotationZ(float a, aiMatrix3x3 arg1) {
    aiMatrix3x3 ret = new aiMatrix3x3(AssimpPINVOKE.aiMatrix3x3_RotationZ(a, aiMatrix3x3.getCPtr(arg1)), false);
    if (AssimpPINVOKE.SWIGPendingException.Pending) throw AssimpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static aiMatrix3x3 Rotation(float a, aiVector3D axis, aiMatrix3x3 arg2) {
    aiMatrix3x3 ret = new aiMatrix3x3(AssimpPINVOKE.aiMatrix3x3_Rotation(a, aiVector3D.getCPtr(axis), aiMatrix3x3.getCPtr(arg2)), false);
    if (AssimpPINVOKE.SWIGPendingException.Pending) throw AssimpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static aiMatrix3x3 Translation(aiVector2D v, aiMatrix3x3 arg1) {
    aiMatrix3x3 ret = new aiMatrix3x3(AssimpPINVOKE.aiMatrix3x3_Translation(aiVector2D.getCPtr(v), aiMatrix3x3.getCPtr(arg1)), false);
    if (AssimpPINVOKE.SWIGPendingException.Pending) throw AssimpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public static aiMatrix3x3 FromToMatrix(aiVector3D from, aiVector3D to, aiMatrix3x3 arg2) {
    aiMatrix3x3 ret = new aiMatrix3x3(AssimpPINVOKE.aiMatrix3x3_FromToMatrix(aiVector3D.getCPtr(from), aiVector3D.getCPtr(to), aiMatrix3x3.getCPtr(arg2)), false);
    if (AssimpPINVOKE.SWIGPendingException.Pending) throw AssimpPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float a1 {
    set {
      AssimpPINVOKE.aiMatrix3x3_a1_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_a1_get(swigCPtr);
      return ret;
    } 
  }

  public float a2 {
    set {
      AssimpPINVOKE.aiMatrix3x3_a2_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_a2_get(swigCPtr);
      return ret;
    } 
  }

  public float a3 {
    set {
      AssimpPINVOKE.aiMatrix3x3_a3_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_a3_get(swigCPtr);
      return ret;
    } 
  }

  public float b1 {
    set {
      AssimpPINVOKE.aiMatrix3x3_b1_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_b1_get(swigCPtr);
      return ret;
    } 
  }

  public float b2 {
    set {
      AssimpPINVOKE.aiMatrix3x3_b2_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_b2_get(swigCPtr);
      return ret;
    } 
  }

  public float b3 {
    set {
      AssimpPINVOKE.aiMatrix3x3_b3_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_b3_get(swigCPtr);
      return ret;
    } 
  }

  public float c1 {
    set {
      AssimpPINVOKE.aiMatrix3x3_c1_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_c1_get(swigCPtr);
      return ret;
    } 
  }

  public float c2 {
    set {
      AssimpPINVOKE.aiMatrix3x3_c2_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_c2_get(swigCPtr);
      return ret;
    } 
  }

  public float c3 {
    set {
      AssimpPINVOKE.aiMatrix3x3_c3_set(swigCPtr, value);
    } 
    get {
      float ret = AssimpPINVOKE.aiMatrix3x3_c3_get(swigCPtr);
      return ret;
    } 
  }

}
