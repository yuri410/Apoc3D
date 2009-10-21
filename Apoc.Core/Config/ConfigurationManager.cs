using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Apoc.Vfs;

namespace Apoc.Config
{
    /// <summary>
    ///  定义不同配置文件格式的读取器接口
    /// </summary>
    public abstract class ConfigurationFormat
    {
        public abstract string[] Filters { get; }

        public abstract Configuration Load(ResourceLocation rl);

        public Configuration Load(string file)
        {
            return Load(new FileLocation(file));
        }
    }

    /// <summary>
    ///  配置管理器。
    ///  负责从文件创建配置(即<see cref="Configuration"/>)，并管理
    /// </summary>
    public class ConfigurationManager : Singleton
    {
        static ConfigurationManager singleton;

        public static ConfigurationManager Instance
        {
            get
            {
                return singleton;
            }
        }

        public static void Initialize()
        {
            if (singleton == null)
            {
                singleton = new ConfigurationManager();
            }
        }

        Dictionary<string, ConfigurationFormat> formats = 
            new Dictionary<string, ConfigurationFormat>(CaseInsensitiveStringComparer.Instance);

        private ConfigurationManager() { }

        /// <summary>
        ///  注册一个配置文件格式
        /// </summary>
        /// <param name="fmt"></param>
        public void Register(ConfigurationFormat fmt)
        {
            string[] exts = fmt.Filters;
            for (int i = 0; i < exts.Length; i++)
            {
                formats.Add(exts[i], fmt);
            }
        }

        /// <summary>
        ///  从文件创建一个配置(<see cref="Configuration"/>)
        /// </summary>
        /// <param name="fl">表示文件位置的<see cref="FileLocation"/></param>
        /// <returns>一个<see cref="Configuration"/>，表示创建好的配置</returns>
        public Configuration CreateInstance(FileLocation fl)
        {
            string ext = Path.GetExtension(fl.Path);

            ConfigurationFormat fmt;
            if (formats.TryGetValue(ext, out fmt))
            {
                return fmt.Load(fl);
            }
            throw new NotSupportedException(ext);
        }

        /// <summary>
        ///  从文件创建一个配置(<see cref="Configuration"/>)
        /// </summary>
        /// <param name="file">文件的路径</param>
        /// <returns>一个<see cref="Configuration"/>，表示创建好的配置</returns>
        public Configuration CreateInstance(string file)
        {
            string ext = Path.GetExtension(file);

            ConfigurationFormat fmt;
            if (formats.TryGetValue(ext, out fmt))
            {
                return fmt.Load(file);
            }
            throw new NotSupportedException(ext);
        }

        /// <summary>
        ///  以指定的类型创建一个配置
        /// </summary>
        /// <param name="rl">指向配置的<see cref="ResourceLocation"/></param>
        /// <param name="type">指定的类型，该值必须与<see cref="ConfigurationFormat"/>中Filter的值一致，不区分大小写</param>
        /// <returns></returns>
        public Configuration CreateInstance(ResourceLocation rl, string type)
        {           
            ConfigurationFormat fmt;
            if (formats.TryGetValue(type, out fmt))
            {
                return fmt.Load(rl);
            }
            throw new NotSupportedException(type);
        }

        protected override void dispose()
        {
        }
    }
}
