using Editor.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Editor.GameProject
{
    [DataContract]
    public class ProjectTemplate
    {
        [DataMember]
        public string ProjectType { get; set; } = string.Empty;
        [DataMember]
        public string ProjectFiles { get; set; } = string.Empty;
        [DataMember]
        public List<string> Folders { get; set; } = new();
        public byte[] Icon { get; set; } = Array.Empty<byte>();
        public byte[] Screenshot { get; set; } = Array.Empty<byte>();
        public string IconFilePath { get; set; } = string.Empty;
        public string ScreenshotFilePath { get; set;} = string.Empty;
        public string ProjectFilePath {  get; set; } = string.Empty;
    }


    class NewProject : ViewModelBase
    {
        private readonly string _templatePath = @"..\..\Editor\ProjectTemplates";

        private string _projectName = "NewProject";
        public string ProjectName 
        {
            get => _projectName;
            set 
            { 
                if (value != _projectName)
                {
                    _projectName = value;
                    ValidateProjectPath();
                    OnPropertyChanged(nameof(ProjectName));
                }
            } 
        }

        private string _projectPath = $@"{Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)}\ThomasProject\";
        public string ProjectPath
        {
            get => _projectPath;
            set
            {
                if (value != _projectPath)
                {
                    _projectPath = value;
                    ValidateProjectPath();
                    OnPropertyChanged(nameof(ProjectPath));
                }
            }
        }

        private bool _isValid;
        public bool IsValid
        {
            get => _isValid;
            set
            {
                if (value != _isValid)
                {
                    _isValid = value;
                    OnPropertyChanged(nameof(IsValid));
                }
            }
        }

        private string _errorMsg;
        public string ErrorMsg
        {
            get => _errorMsg;
            set
            {
                if (value != _errorMsg)
                {
                    _errorMsg = value;
                    OnPropertyChanged(nameof(ErrorMsg));
                }
            }
        }

        private ObservableCollection<ProjectTemplate> _projectTemplate = new ObservableCollection<ProjectTemplate>();
        public ReadOnlyObservableCollection<ProjectTemplate> ProjectTemplates
        {
            get;
        }

        private bool ValidateProjectPath()
        {
            var path = ProjectPath;
            if (!Path.EndsInDirectorySeparator(path)) path += @"\";

            path += $@"{ProjectName}\";

            IsValid = false;
            if (string.IsNullOrWhiteSpace(ProjectName.Trim()))
            {
                ErrorMsg = "Type in a project name";
            }
            else if (ProjectName.IndexOfAny(Path.GetInvalidFileNameChars()) != -1)
            {
                ErrorMsg = "Invalid character(s) used in project name";
            }
            else if(string.IsNullOrWhiteSpace(ProjectPath.Trim()))
            {
                ErrorMsg = "Select a valid Project folder";
            }
            else if (ProjectPath.IndexOfAny(Path.GetInvalidPathChars()) != -1)
            {
                ErrorMsg = "Invalid character(s) used in project path";
            }
            else if (Directory.Exists(path) && Directory.EnumerateFileSystemEntries(path).Any())
            {
                ErrorMsg = "Selected project folder already exist and is not empty";
            }
            else
            {
                ErrorMsg=string.Empty;
                IsValid = true;
            }

            return IsValid;
        }

        public string CreateProject(ProjectTemplate template)
        {
            ValidateProjectPath();

            if (!IsValid)
            {
                return string.Empty;
            }

            if (!Path.EndsInDirectorySeparator(ProjectPath)) ProjectPath += @"\";
            var path = $@"{ProjectPath}{ProjectName}\";

            try
            {
                if (Directory.Exists (path)) Directory.CreateDirectory(path);

                foreach (var folder in template.Folders)
                {
                    Directory.CreateDirectory(Path.GetFullPath(Path.Combine(Path.GetDirectoryName(path), folder)));
                }

                var dirInfo = new DirectoryInfo(path + @".Thomas\");
                dirInfo.Attributes |= FileAttributes.Hidden;

                File.Copy(template.IconFilePath, Path.GetFullPath(Path.Combine(dirInfo.FullName, "icon.png")));
                File.Copy(template.ScreenshotFilePath, Path.GetFullPath(Path.Combine(dirInfo.FullName, "Screenshot.png")));

                var projectXml = File.ReadAllText(template.ProjectFilePath);
                projectXml = string.Format(projectXml, ProjectName, ProjectPath);
                var projectPath = Path.GetFullPath(Path.Combine(path, $"{ProjectName}{Project.Extension}"));
                File.WriteAllText(projectPath, projectXml);

                return path;

            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                Logger.Log(MessageType.Error, $"Failed to create project {ProjectName}");
                return string.Empty;
            }

        }

        public NewProject()
        {
            ProjectTemplates = new ReadOnlyObservableCollection<ProjectTemplate>(_projectTemplate);
            try
            {
                var templatesFiles = Directory.GetFiles(_templatePath, "Template.xml", SearchOption.AllDirectories);
                Debug.Assert(templatesFiles.Any());
                foreach (var file in templatesFiles)
                {
                    var template = Serializer.FromFile<ProjectTemplate>(file);
                    template.IconFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), "icon.png"));
                    template.Icon = File.ReadAllBytes(template.IconFilePath);
                    template.ScreenshotFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), "Screenshot.png"));
                    template.Screenshot = File.ReadAllBytes(template.ScreenshotFilePath);
                    template.ProjectFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), template.ProjectFiles));



                    _projectTemplate.Add(template);
                }
                ValidateProjectPath();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                Logger.Log(MessageType.Error, $"Failed to read project templates");
                throw;
            }
        }
    }
}
