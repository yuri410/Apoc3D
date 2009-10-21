using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle;
using VirtualBicycle.UI;

namespace VirtualBicycle.Ide.Designers
{
    public partial class frmCsfEntryEdit : Form
    {
        static char[] WhitespaceChars = new char[] { 
            '\t', '\n', '\v', '\f', '\r', ' ', '\x0085', '\x00a0', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
            ' ', ' ', ' ', ' ', '​', '\u2028', '\u2029', '　', '﻿'
            };

        public bool isCanceled;

        public string content;
        public string name;
        public string extraData;
        StringTable translateData;

        public frmCsfEntryEdit(StringTable st)
        {
            InitializeComponent();

            translateData = st;
            LanguageParser.ParseLanguage(st, this);
        }

        private void frmCsfInsert_Load(object sender, EventArgs e)
        {
            textBox1.Text = name;
            textBox3.Text = content;
            textBox2.Text = extraData;
            isCanceled = true;

        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            isCanceled = false;

            name = textBox1.Text;
            extraData = textBox2.Text;
            content = textBox3.Text;
            this.Close();
        }

        private void textBox1_Validating(object sender, CancelEventArgs e)
        {
            string text = textBox1.Text;

            if (text.StartsWith(" ") | text.EndsWith(" "))
            {
                e.Cancel = true;
                MessageBox.Show(this, translateData["MSG:CSFNameSpaceSE"], translateData["GUI:Error"], MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (text.StartsWith(CsfDesigner.NoCategory))
            {
                e.Cancel = true;
                MessageBox.Show(this, translateData["MSG:CSFStartNC"], translateData["GUI:Error"], MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            for (int i = 0; i < text.Length; i++)
                if (text[i] > 255 || text[i] < 0)
                {
                    e.Cancel = true;
                    MessageBox.Show(this, translateData["MSG:CSFExtraDataLimit"], translateData["GUI:Error"], MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
        }

        private void textBox2_Validating(object sender, CancelEventArgs e)
        {
            string text = textBox2.Text;
            for (int i = 0; i < text.Length; i++)           
                if (text[i] > 255 || text[i] < 0)
                {
                    e.Cancel = true;
                    MessageBox.Show(this, translateData["MSG:CSFExtraDataLimit"], translateData["GUI:Error"], MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
            
        }

        private void frmCsfEntryEdit_Shown(object sender, EventArgs e)
        {
            isCanceled = true;
        }


    }
}
