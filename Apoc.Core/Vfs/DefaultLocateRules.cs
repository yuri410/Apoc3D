using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Vfs
{
    public class DefaultLocateRules : Singleton
    {
        static DefaultLocateRules singleton;

        public static DefaultLocateRules Instance
        {
            get
            {
                if (singleton == null) 
                {
                    singleton = new DefaultLocateRules();
                }
                return singleton;
            }
        }

        private DefaultLocateRules()
        {
        }

        public FileLocateRule RootRule
        {
            get;
            set;
        }

        public FileLocateRule TextureLocateRule
        {
            get;
            set;
        }
        public FileLocateRule PdbSymbolLocateRule
        {
            get;
            set;
        }

        protected override void dispose()
        {
            TextureLocateRule = null;
        }
    }
}
