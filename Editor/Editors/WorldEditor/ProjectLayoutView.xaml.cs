using Editor.Components;
using Editor.GameProject;
using Editor.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Editor.Editors
{
    /// <summary>
    /// Interaction logic for ProjectLayoutView.xaml
    /// </summary>
    public partial class ProjectLayoutView : UserControl
    {
        public ProjectLayoutView()
        {
            InitializeComponent();
        }

        private void OnAddGameEntity_Button_Click(object sender, RoutedEventArgs e)
        {
            var btn = sender as Button;
            var vm = btn.DataContext as Scene;
            vm.AddGameEntityCommand.Execute(new GameEntity(vm) {  Name = "Empty Game Entity"});
        }

        private void OnGameEntity_ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

            GameEntityView.Instance.DataContext = null; 
            if (e.AddedItems.Count > 0)
            {
                GameEntityView.Instance.DataContext = (sender as ListBox).SelectedItems[0];
            }

            var listBox = sender as ListBox;
            var newSelection = listBox.SelectedItems.Cast<GameEntity>().ToList();
            var previousSelection = newSelection.Except(e.AddedItems.Cast<GameEntity>()).Concat(e.RemovedItems.Cast<GameEntity>()).ToList();

            Project.UndoRedo.Add(new UndoRedoAction(
                // Undo
                () =>
                {
                    listBox.UnselectAll();
                    foreach (var item in previousSelection)
                    {
                        var container = listBox.ItemContainerGenerator.ContainerFromItem(item) as ListBoxItem;
                        if (container != null)
                            container.IsSelected = true;
                    }
                },
                // Redo
                () =>
                {
                    listBox.UnselectAll();
                    foreach (var item in newSelection)
                    {
                        var container = listBox.ItemContainerGenerator.ContainerFromItem(item) as ListBoxItem;
                        if (container != null)
                            container.IsSelected = true;
                    }
                },
                "Selection changed"
            ));
        }
    }
}
