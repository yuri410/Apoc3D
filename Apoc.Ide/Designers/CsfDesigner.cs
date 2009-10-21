using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Drawing.Design;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using VirtualBicycle;
using VirtualBicycle.IO;
using VirtualBicycle.UI;
using WeifenLuo.WinFormsUI.Docking;

namespace VirtualBicycle.Ide.Designers
{
    public partial class CsfDesigner : GeneralDocumentBase
    {
        public const string Extension = ".csf";

        internal class Entry
        {
            static char[] WhitespaceChars = new char[]
            { 
                    '\t', '\n', '\v', '\f', '\r', ' ', '\x0085', '\x00a0', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
                    ' ', ' ', ' ', ' ', '​', '\u2028', '\u2029', '　', '﻿'
            };

            CsfDesigner document;

            public string cate;
            public Category parent;

            public string name;
            public string text;
            public string extra;

            public Entry(CsfDesigner doc)
            {
                document = doc;
            }

            public override string ToString()
            {
                StringBuilder sb = new StringBuilder(7);
                sb.Append("{ CSF: ");
                sb.Append(DevStringTable.Instance["PROP:CSFNAME"]);
                sb.Append("=");
                sb.Append(DevStringTable.Instance["PROP:CSFContext"]);
                sb.Append("=");
                sb.Append(text);
                sb.Append("}");
                return sb.ToString();
            }


            [LocalizedDescription("PROP:CSfNAME")]
            public string Name
            {
                get { return name; }
                set
                {
                    if (value.StartsWith(" ") | value.EndsWith(" "))
                    {
                        throw new Exception(DevStringTable.Instance["MSG:CSFNameSpaceSE"]);
                    }

                    if (value.StartsWith(CsfDesigner.NoCategory))
                    {
                        throw new Exception(DevStringTable.Instance["MSG:CSFStartNC"]);
                    }

                    for (int i = 0; i < value.Length; i++)
                        if (value[i] > 255 || value[i] < 0)
                        {
                            throw new Exception(DevStringTable.Instance["MSG:CSFExtraDataLimit"]);
                        }

                    document.EditEntry(this, value, text, extra);
                    name = value;
                }
            }
            [LocalizedDescription("PROP:CSFTEXT")]
            [Editor(typeof(MultilineStringEditor), typeof(UITypeEditor))]
            public string Text
            {
                get { return text; }
                set
                {
                    document.EditEntry(this, name, value, extra);
                    text = value;
                }
            }
            [LocalizedDescription("PROP:CSFEXTRA")]
            public string ExtraData
            {
                get { return extra; }
                set
                {
                    for (int i = 0; i < value.Length; i++)
                        if (value[i] > 255 || value[i] < 0)
                        {
                            throw new Exception(DevStringTable.Instance["MSG:CSFExtraDataLimit"]);
                        }
                    document.EditEntry(this, name, text, value);
                    extra = value;
                }
            }

        }
        internal class Category
        {
            public string name;
            public List<Entry> data;
        }

        class CategoryComparer : IComparer<Category>
        {
            #region IComparer<Category> 成员

            public int Compare(Category x, Category y)
            {
                return x.name.CompareTo(y.name);
            }

            #endregion
        }

        public const string NoCategory = "---";

        List<Category> fileData;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="file"></param>
        public CsfDesigner(DesignerAbstractFactory fac, ResourceLocation res)
        {
            InitializeComponent();

            LanguageParser.ParseLanguage(DevStringTable.Instance, this);
            LanguageParser.ParseLanguage(DevStringTable.Instance, toolStrip1);
            LanguageParser.ParseLanguage(DevStringTable.Instance, listView1);
            LanguageParser.ParseLanguage(DevStringTable.Instance, listView2);
            Init(fac, res);
        }
        private void CsfDocument_Load(object sender, EventArgs e)
        {
            splitContainer1.SplitterDistance = CsfDesignerConfigs.Instance.SpliterDistVert;
            splitContainer2.SplitterDistance = CsfDesignerConfigs.Instance.SpliterDistHoz;



            Saved = true;
        }

        public override ToolStrip[] ToolStrips
        {
            get
            {
                return new ToolStrip[] { toolStrip1 };
            }
        }

        string GetCategory(string name)
        {
            string[] part = name.Split(':', '.', '_');

            if (part.Length > 1)
                return part[0];
            else
                return NoCategory;
        }
        public override bool LoadRes()
        {
            if (ResourceLocation != null)
            {
                StringTable data = new StringTableCsfFormat().Load(ResourceLocation);

                List<Entry> list = new List<Entry>(data.Count);

                foreach (KeyValuePair<string, KeyValuePair<string, string>> entry in data)
                {
                    Entry ent = new Entry(this);
                    ent.name = entry.Key;
                    ent.text = entry.Value.Value;
                    ent.extra = entry.Value.Key;
                    ent.cate = GetCategory(ent.name);

                    list.Add(ent);
                }
                list.Sort(this.Comparsion);
                list.Add(new Entry(this));

                fileData = new List<Category>();

                if (list.Count > 0)
                {
                    Category currentCate = new Category();
                    currentCate.data = new List<Entry>();
                    currentCate.name = list[0].cate;

                    for (int i = 0; i < list.Count; i++)
                    {
                        if (list[i].cate != currentCate.name)
                        {
                            fileData.Add(currentCate);

                            currentCate = new Category();
                            currentCate.name = list[i].cate;
                            currentCate.data = new List<Entry>();
                        }

                        list[i].parent = currentCate;
                        currentCate.data.Add(list[i]);
                    }
                }
                ListCategory();

                fileData.Sort(new CategoryComparer());
                Saved = true;
                return true;
            }
            fileData = new List<Category>();
            Saved = true;
            return true;
        }
        int Comparsion(Entry a, Entry b)
        {
            return a.name.CompareTo(b.name);
        }

        public override bool SaveRes()
        {
            if (ResourceLocation.IsReadOnly)
                throw new InvalidOperationException();

            Dictionary<string, KeyValuePair<string, string>> data = new Dictionary<string, KeyValuePair<string, string>>();

            //List<KeyValuePair<string, KeyValuePair<string, string>>> sounds = 
            //    new List<KeyValuePair<string, KeyValuePair<string, string>>>();

            for (int i = 0; i < fileData.Count; i++)
            {
                for (int j = 0; j < fileData[i].data.Count; j++)
                {
                    data.Add(fileData[i].data[j].name,
                        new KeyValuePair<string, string>(fileData[i].data[j].extra, fileData[i].data[j].text));
                }
            }
            //for (int i = 0; i < fileData.Count; i++)
            //    for (int j = 0; j < fileData[i].sounds.Count; j++)
            //        sounds.Add(
            //            new KeyValuePair<string, KeyValuePair<string, string>>(
            //                fileData[i].sounds[j].name,
            //                new KeyValuePair<string, string>(fileData[i].sounds[j].extra, fileData[i].sounds[j].text)));
                

            //FileStream fs = new FileStream(sFile, FileMode.OpenOrCreate, FileAccess.Write, FileShare.Write);
            Stream stm = ResourceLocation.GetStream;
            try
            {
                stm.SetLength(0);
            }
            catch (NotSupportedException)
            {
            }

            StringTableCsfFormat fmt = new StringTableCsfFormat();
            fmt.Write(data, stm);

            //BinaryWriter bw = new BinaryWriter(stm);

            //bw.Write((int)FileID.Csf);
            //bw.Write((int)3);
            //bw.Write(sounds.Count);
            //bw.Write(sounds.Count);
            //bw.Write((int)0);
            //bw.Write((int)0);

            //for (int i = 0; i < sounds.Count; i++)
            //{
            //    string key = sounds[i].Key;
            //    string text = sounds[i].Value.Value;
            //    string ext = sounds[i].Value.Key;

            //    bw.Write((int)CsfHeader.LabelID);

            //    if (text != string.Empty | ext != string.Empty)
            //    {
            //        bw.Write((int)1);

            //        bw.Write(key.Length);
            //        bw.Write(key.ToCharArray());

            //        bool hasExtra = !string.IsNullOrEmpty(ext);

            //        if (hasExtra)
            //            bw.Write((int)CsfHeader.WStringID);
            //        else
            //            bw.Write((int)CsfHeader.StringID);

            //        bw.Write(text.Length);

            //        for (int j = 0; j < text.Length; j++)
            //        {
            //            ushort v = (ushort)text[j];

            //            bw.Write((byte)~(v & 255));
            //            bw.Write((byte)~(v >> 8));
            //        }

            //        if (hasExtra)
            //        {
            //            bw.Write(ext.Length);
            //            bw.Write(ext.ToCharArray());
            //        }
            //    }
            //    else
            //    {
            //        bw.Write((int)0);
            //        bw.Write(key.Length);
            //        bw.Write(key.ToCharArray());
            //    }
                
            //}

            //bw.Close();

            data.Clear();

            Saved = true;
            return true;
            //throw new NotImplementedException();
        }


        /// <summary>
        /// 列出所有类别
        /// </summary>
        void ListCategory()
        {
            listView1.Sorting = SortOrder.None;
            listView1.Items.Clear();

            for (int i = 0; i < fileData.Count; i++)
            {
                ListViewItem itm = listView1.Items.Add(fileData[i].name, fileData[i].name, 0);
                itm.Tag = fileData[i];
                itm.SubItems.Add(fileData[i].data.Count.ToString());
            }
            listView1.Sorting = SortOrder.Ascending;

        }
        void ListCategory(Category selCate)
        {
            listView1.Sorting = SortOrder.None;
            listView1.Items.Clear();

            for (int i = 0; i < fileData.Count; i++)
            {
                ListViewItem itm = listView1.Items.Add(fileData[i].name, fileData[i].name, 0);
                itm.Tag = fileData[i];
                itm.SubItems.Add(fileData[i].data.Count.ToString());

                if (fileData[i] == selCate)
                    itm.Selected = true;
            }
            listView1.Sorting = SortOrder.Ascending;
        }

        /// <summary>
        /// 列出cat中所有的项
        /// </summary>
        /// <param name="cat"></param>
        void ListEntry(Category cat)
        {
            listView2.Sorting = SortOrder.None;
            listView2.Items.Clear();
            for (int i = 0; i < cat.data.Count; i++)
            {
                ListViewItem itm = listView2.Items.Add(cat.data[i].name);
                itm.Tag = cat.data[i];
                itm.SubItems.Add(cat.data[i].text);
                itm.SubItems.Add(cat.data[i].extra);
            }
            listView2.Sorting = SortOrder.Ascending;
        }
        void ListEntry(Category cat, Entry selectedEntry)
        {
            listView2.Sorting = SortOrder.None;
            listView2.Items.Clear();
            for (int i = 0; i < cat.data.Count; i++)
            {
                ListViewItem itm = listView2.Items.Add(cat.data[i].name);
                itm.Tag = cat.data[i];
                itm.SubItems.Add(cat.data[i].text);
                itm.SubItems.Add(cat.data[i].extra);

                itm.Selected = cat.data[i] == selectedEntry;
            }
            listView2.Sorting = SortOrder.Ascending;
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count > 0)
                ListEntry((Category)listView1.SelectedItems[0].Tag);

            EditDelToolEnable = false;
            //isMCListed = false;
        }
        private void listView2_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView2.SelectedItems.Count > 0)
            {
                textBox1.Text = ((Entry)listView2.SelectedItems[0].Tag).text;
                if (propertyUpdated != null)
                {
                    Entry ent = (Entry)listView2.SelectedItems[0].Tag;
                    propertyUpdated(ent, null);
                }
                EditDelToolEnable = true;
            }
            else
            {
                if (propertyUpdated != null)
                {
                    propertyUpdated(null, null);
                }
                EditDelToolEnable = false;
            }
        }

        bool EditDelToolEnable
        {
            set
            {
                editEntryTool.Enabled = value;
                delEntryTool.Enabled = value;
            }
        }

        Category Find(string cate)
        {
            for (int i = 0; i < fileData.Count; i++)
                if (fileData[i].name == cate)
                    return fileData[i];

            throw new Exception();
        }


        bool CheckEntryName(string name, int limit)
        {
            limit--;

            if (name == string.Empty)
            {
                MessageBox.Show(this, DevStringTable.Instance["Error:CSFEmptyName"], DevStringTable.Instance["GUI:Error"], MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            int times = 0;
            for (int i = 0; i < fileData.Count; i++)
            {
                for (int j = 0; j < fileData[i].data.Count; j++)
                {
                    if (CaseInsensitiveStringComparer.Compare(fileData[i].data[j].name, name))
                    {
                        times++;
                        if (times > limit)
                        {
                            MessageBox.Show(this, DevStringTable.Instance["Error:CSFDupName"].Replace("{0}", name), DevStringTable.Instance["GUI:Error"], MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return false;
                        }
                    }
                }
            }
            return true;
        }
        bool EditEntry(Entry ent, out bool isNewCate, IWindowsFormsEditorService edSvc)
        {
            frmCsfEntryEdit f = new frmCsfEntryEdit(DevStringTable.Instance);
            //f.Text = Program.StringTable["GUI:CSFEdit"];
            f.name = ent.name;
            f.content = ent.text;
            f.extraData = ent.extra;

            if (edSvc == null)
            {
                f.ShowDialog(this);
            }
            else
            {
                edSvc.ShowDialog(f);
            }
            
            while (!f.isCanceled && !CheckEntryName(f.name, 2))
            {
                if (edSvc == null)
                {
                    f.ShowDialog(this);
                }
                else
                {
                    edSvc.ShowDialog(f);
                }
            }

            isNewCate = false;
            if (!f.isCanceled)
            {
                string newCate = GetCategory(f.name);

                // 类别是否相同？
                if (newCate != ent.cate)
                {
                    isNewCate = true;

                    // 从旧类别中去除
                    ent.parent.data.Remove(ent);
                    FlushCategory(ent.parent);

                    ent.cate = newCate;

                    // 新类别是否存在？
                    try
                    {
                        ent.parent = Find(newCate);
                    }
                    catch
                    {
                        Category newCat = new Category();
                        newCat.name = newCate;
                        newCat.data = new List<Entry>();
                        fileData.Add(newCat);

                        ent.parent = newCat;
                    }

                    ent.parent.data.Add(ent);
                }

                ent.name = f.name;
                ent.extra = f.extraData;
                ent.text = f.content;

                f.Dispose();
                return true;
            }

            f.Dispose();
            return false;
        }
        bool EditEntry(Entry ent, out bool isNewCate)
        {
            return EditEntry(ent, out isNewCate, null);
        }
        void EditEntry()
        {
            bool isNewCate;
            Entry item = (Entry)listView2.SelectedItems[0].Tag;
            if (EditEntry(item, out isNewCate))
            {
                if (isNewCate)
                {
                    //isMCListed = true;
                    ListCategory();//item.parent
                    ListEntry(item.parent, item);
                }
                else
                {
                    listView2.SelectedItems[0].Text = item.name;
                    listView2.SelectedItems[0].SubItems[1].Text = item.text;
                    listView2.SelectedItems[0].SubItems[2].Text = item.extra;
                }
                
                if (propertyUpdated != null)
                {                   
                    propertyUpdated(item, null);
                }
                Saved = false;
            }
        }

        void EditEntry(Entry ent, string name, string text, string extra)
        {
            bool isNewCate = false;

            string newCate = GetCategory(name);

            // 类别是否相同？
            if (newCate != ent.cate)
            {
                isNewCate = true;

                // 从旧类别中去除
                ent.parent.data.Remove(ent);
                FlushCategory(ent.parent);

                ent.cate = newCate;

                // 新类别是否存在？
                try
                {
                    ent.parent = Find(newCate);
                }
                catch
                {
                    Category newCat = new Category();
                    newCat.name = newCate;
                    newCat.data = new List<Entry>();
                    fileData.Add(newCat);

                    ent.parent = newCat;
                }

                ent.parent.data.Add(ent);
            }


            if (isNewCate)
            {
                //isMCListed = true;
                ListCategory();//item.parent
                ListEntry(ent.parent, ent);
            }
            else
            {

                listView2.SelectedItems[0].Text = name;
                listView2.SelectedItems[0].SubItems[1].Text = text;
                listView2.SelectedItems[0].SubItems[2].Text = extra;
            }
            Saved = false;
        }

        void FlushCategory(Category cat)
        {
            if (cat.data.Count == 0)
                fileData.Remove(cat);
        }

        private void listView2_DoubleClick(object sender, EventArgs e)
        {
            if (listView2.SelectedItems.Count > 0)
                EditEntry();
        }

        private void editEntryTool_Click(object sender, EventArgs e)
        {
            EditEntry();
        }
        private void newEntryTool_Click(object sender, EventArgs e)
        {
            string defCate;

            if (listView1.SelectedItems.Count > 0)
            {
                defCate = ((Category)listView1.SelectedItems[0].Tag).name;
                if (defCate == NoCategory)
                    defCate = string.Empty;
            }
            else
                defCate = String.Empty;

            // ==============================================================

            frmCsfEntryEdit f = new frmCsfEntryEdit(DevStringTable.Instance);
            //f.Text = translateData["GUI:CSFNewEntry"];
            f.name = defCate;
            f.content = string.Empty;
            f.extraData = string.Empty;

            f.ShowDialog(this);

            while (!f.isCanceled && !CheckEntryName(f.name, 1))
                f.ShowDialog(this);

            Entry ent = new Entry(this);

            if (!f.isCanceled)
            {
                string newCate = GetCategory(f.name);
                ent.cate = newCate;

                bool cateExists = true;
                // 类别是否存在？
                try
                {
                    ent.parent = Find(newCate);
                }
                catch
                {
                    Category newCat = new Category();
                    newCat.name = newCate;
                    newCat.data = new List<Entry>();
                    fileData.Add(newCat);

                    ent.parent = newCat;

                    cateExists = false;
                }

                ent.parent.data.Add(ent);

                ent.name = f.name;
                ent.extra = f.extraData;
                ent.text = f.content;

                if (!cateExists | newCate != defCate)
                {
                    //isMCListed = true;
                    ListCategory();//ent.parent
                    ListEntry(ent.parent, ent);
                }
                else
                {
                    ListViewItem itm = listView2.Items.Add(ent.name);
                    itm.Tag = ent;
                    itm.SubItems.Add(ent.text);
                    itm.SubItems.Add(ent.extra);
                    itm.Selected = true;

                    ListViewItem[] cis = listView1.Items.Find(defCate, false);

                    if (cis.Length > 0)
                        cis[0].SubItems[1].Text = (int.Parse(cis[0].SubItems[1].Text) + 1).ToString();
                }

                Saved = false;
            }

            f.Dispose();
        }
        private void delEntryTool_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(
                this, DevStringTable.Instance["MSG:EnsureDelete"], DevStringTable.Instance["GUI:Delete"], MessageBoxButtons.YesNo, MessageBoxIcon.Question) ==
                DialogResult.Yes)
            {
                for (int i = listView2.SelectedItems.Count - 1; i >= 0; i--)
                {
                    Entry ent = (Entry)listView2.SelectedItems[i].Tag;
                    ent.parent.data.Remove(ent);
                    FlushCategory(ent.parent);

                    listView2.SelectedItems[i].Remove();
                }

                ListCategory();
            }
        }

        private void CsfDocument_FormClosing(object sender, FormClosingEventArgs e)
        {
            CsfDesignerConfigs.Instance.SpliterDistVert = splitContainer1.SplitterDistance;
            CsfDesignerConfigs.Instance.SpliterDistHoz = splitContainer2.SplitterDistance;
        }
     
    }

}
