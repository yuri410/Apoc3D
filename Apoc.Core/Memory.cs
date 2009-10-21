using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Apoc
{
    /// <summary>
    /// 提供了内存的复制,清零等操作
    /// </summary>
    public unsafe static class Memory
    {
        #region Helper Structs
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct helper102400
        {
            public const int Size = 102400;
            fixed byte buffer[Size];
        }
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct helper10240
        {
            public const int Size = 10240;
            fixed byte buffer[Size];
        }
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct helper1024
        {
            public const int Size = 1024;
            fixed byte buffer[Size];
        }
        #endregion

        #region Copy
        /// <summary>
        /// 从当前内存复制内容到目标内存
        /// </summary>
        /// <param name="src">当前内存指针</param>
        /// <param name="dst">目标内存指针</param>
        /// <param name="size">复制内容的大小</param>
        public static void Copy(IntPtr src, IntPtr dst, int size)
        {
            Copy(src.ToPointer(), dst.ToPointer(), size);
        }
        /// <summary>
        /// 从当前内存复制内容到目标内存
        /// </summary>
        /// <param name="src">当前内存指针</param>
        /// <param name="dst">目标内存指针</param>
        /// <param name="size">复制内容的大小</param>
        public static void Copy(void* sr, void* dst, int size)
        {
            byte* dest = (byte*)dst;
            byte* src = (byte*)sr;

            if (size >= helper102400.Size)
            {
                do
                {
                    *((helper102400*)dest) = *((helper102400*)src);
                    dest += helper102400.Size;
                    src += helper102400.Size;
                }
                while ((size -= helper102400.Size) >= helper102400.Size);
            }
            if (size >= helper10240.Size)
            {
                do
                {
                    *((helper10240*)dest) = *((helper10240*)src);
                    dest += helper10240.Size;
                    src += helper10240.Size;
                }
                while ((size -= helper10240.Size) >= helper10240.Size);
            }
            if (size >= helper1024.Size)
            {
                do
                {
                    *((helper1024*)dest) = *((helper1024*)src);
                    dest += helper1024.Size;
                    src += helper1024.Size;
                }
                while ((size -= helper1024.Size) >= helper1024.Size);
            }
            if (size >= 16)
            {
                do
                {
                    *((long*)dest) = *((long*)src);
                    *((long*)(dest + 8)) = *((long*)(src + 8));
                    dest += 16;
                    src += 16;
                }
                while ((size -= 16) >= 16);
            }
            if ((size & 8) != 0)
            {
                *((long*)dest) = *((long*)src);
                dest += 8;
                src += 8;
            }
            if ((size & 4) != 0)
            {
                *((int*)dest) = *((int*)src);
                dest += 4;
                src += 4;
            }
            if ((size & 2) != 0)
            {
                *((short*)dest) = *((short*)src);
                dest += 2;
                src += 2;
            }
            if ((size & 1) != 0)
            {
                dest++;
                src++;
                dest[0] = src[0];
            }
        }
        #endregion

        #region Zero
        /// <summary>
        /// 对当前内存的内容清零
        /// </summary>
        /// <param name="p">当前内存</param>
        /// <param name="size">清零内容的大小</param>
        public static void Zero(IntPtr p, int size)
        {
            Zero(p.ToPointer(), size);
        }

        public static void Zero(void* p, int size)
        {
            byte* src = (byte*)p;
            while (true)
            {
                size--;
                if (size <= 0)
                {
                    return;
                }
                src[size] = 0;
            }

        }
        #endregion
    }
}
