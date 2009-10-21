using System;
using System.Collections.Generic;
using System.Text;


namespace VBIDE
{
    public class ResourceManager
    {
        static ResourceManager singleton;

        public static ResourceManager Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new ResourceManager();
                }
                return singleton;
            }
        }

        Dictionary<int, ResourceLocation> resources;

        private ResourceManager()
        {
            resources = new Dictionary<int, ResourceLocation>();
        }

        public void Register(ResourceLocation res)
        {
            resources.Add(res.GetHashCode(), res);
        }
        public void Unregister(ResourceLocation res)
        {
            resources.Remove(res.GetHashCode());
        }

    }
}
