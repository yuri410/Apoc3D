using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Core
{
    abstract class ResourceOperation
    {
        Resource resource;

        public Resource Resource
        {
            get { return resource; }
        }

        protected ResourceOperation(Resource res) 
        {
            this.resource = res;
        }

        public abstract void Process();
    }

    class ResourceLoader : ResourceOperation
    {
        public ResourceLoader(Resource resource)
            : base(resource)
        {
        }

        public override void Process()
        {
            if (Resource != null) 
            {
                Resource.Load();
            }
        }
    }
    class ResourceUnloader : ResourceOperation
    {
        public ResourceUnloader(Resource resource)
            : base(resource)
        {

        }

        public override void Process()
        {
            if (Resource != null)
            {
                Resource.Unload();
            }
        }
    }

    class AsyncLoader
    {
        Queue<ResourceOperation> opQueue;

        object syncHelper = new object();

        public AsyncLoader() 
        {
            opQueue = new Queue<ResourceOperation>();
        }

        private void Main() 
        {
            while (true) 
            {
                ResourceOperation resOp = null;

                lock (syncHelper) 
                {
                    resOp = opQueue.Dequeue();
                }

                resOp.Process();
            }
        }
    }
}
