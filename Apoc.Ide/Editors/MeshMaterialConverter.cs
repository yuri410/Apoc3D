using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using VirtualBicycle.Ide.Editors.EditableObjects;

namespace VirtualBicycle.Ide.Editors
{
    public class MeshMaterialConverter : ExpandableObjectConverter
    {
        public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
        {
            string[] props = new string[]
            {
                "Texture1", "Texture1Embeded", "TextureFile1",
                "Texture2", "Texture2Embeded", "TextureFile2",
                "Texture3", "Texture3Embeded", "TextureFile3",
                "Texture4", "Texture4Embeded", "TextureFile4",
                "Flags", "IsTwoSided", "IsTransparent",
                "Power", "Ambient", "Diffuse", "Specular", "Emissive" 
            };

            return TypeDescriptor.GetProperties(typeof(EditableMeshMaterial), attributes).Sort(props);
        }
        public override bool GetPropertiesSupported(ITypeDescriptorContext context)
        {
            return true;
        }
    }
}
