using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide.Converters
{
    /// <summary>
    /// 用于转换数据的转换器
    /// </summary>
    public abstract class ConverterBase
    {
        protected Icon icon;        

        public abstract void ShowDialog(object sender, EventArgs e);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="source"></param>
        /// <param name="dest"></param>
        /// <remarks>将参数设计成属性</remarks>
        public abstract void Convert(ResourceLocation source, ResourceLocation dest);

        public abstract string Name
        {
            get;
        }

        public Icon GetIcon
        {
            get
            {
                if (icon == null)
                {
                    return Program.DefaultIcon;
                }
                return icon;                                                 
            }
        }

        
        public abstract string[] SourceExt { get; }
        public abstract string[] DestExt { get; }

        /// <summary>
        ///  获取原数据的描述
        /// </summary>
        public abstract string SourceDesc { get; }
        /// <summary>
        ///  获取目标数据的描述
        /// </summary>
        public abstract string DestDesc { get; }

        public string  GetOpenFilter()
        {
            return DevUtils.GetFilter(SourceDesc, SourceExt);
        }
        public string GetSaveFilter()
        {
            return DevUtils.GetFilter(DestDesc, DestExt);
        }
    }


}
