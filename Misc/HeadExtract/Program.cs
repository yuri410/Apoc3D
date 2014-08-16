using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace HeadExtract
{
    // This is a tool that extracts and copies a header tree.
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            string srcPath = string.Empty;
            string destPath = string.Empty;

            if (args.Length != 2)
            {
                Console.WriteLine("Usage:");
                Console.WriteLine("HeadExtract srcFolder destFolder");
                Console.WriteLine();
                Console.WriteLine("Press Y to manually select folders, any other key to exit.");

                ConsoleKeyInfo keyInfo = Console.ReadKey();
                if (keyInfo != null && keyInfo.Key == ConsoleKey.Y)
                {
                    FolderBrowserDialog dlg = new FolderBrowserDialog();
                    dlg.Description = "Source folder";
                    if (dlg.ShowDialog(null) == DialogResult.OK)
                    {
                        srcPath = dlg.SelectedPath;
                    }

                    dlg.Description = "Destination folder";
                    if (dlg.ShowDialog(null) == DialogResult.OK)
                    {
                        destPath = dlg.SelectedPath;   
                    }
                }
            }
            else
            {
                srcPath = args[0];
                destPath = args[1];
            }

            if (srcPath.Length > 0 && destPath.Length > 0)
            {
                Console.WriteLine("Processing...");
               
                string[] files = Directory.GetFiles(srcPath, "*.h", SearchOption.AllDirectories);
                Copy(srcPath, destPath, files);

                files = Directory.GetFiles(srcPath, "*.hpp", SearchOption.AllDirectories);
                Copy(srcPath, destPath, files);

                files = Directory.GetFiles(srcPath, "*.inl", SearchOption.AllDirectories);
                Copy(srcPath, destPath, files);
            }
        }

        static void Copy(string _srcPath, string _destPath, string[] files)
        {
            string srcPath = _srcPath;

            if (!srcPath.EndsWith(Path.DirectorySeparatorChar.ToString()))
            {
                srcPath = _srcPath + Path.DirectorySeparatorChar.ToString();
            }

            for (int i = 0; i < files.Length; i++)
            {
                string path = Path.GetDirectoryName(files[i]);

                string relPath = string.Empty;
                if (path.Length > srcPath.Length)
                {
                    relPath = path.Substring(srcPath.Length);
                }

                string destPath = Path.Combine(_destPath, relPath);

                if (!Directory.Exists(destPath))
                {
                    Directory.CreateDirectory(destPath);
                }

                destPath = Path.Combine(destPath, Path.GetFileName(files[i]));

                File.Copy(files[i], destPath);
            }
        }
    }
}
