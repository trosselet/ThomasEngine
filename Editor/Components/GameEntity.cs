﻿using Editor.GameProject;
using Editor.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace Editor.Components
{
    [DataContract]
    [KnownType(typeof(Transform))]
    class GameEntity : ViewModelBase
    {
        private bool _isEnabled;
        [DataMember]
        public bool IsEnabled
        {
            get => _isEnabled;

            set
            {
                if (_isEnabled != value)
                {
                    _isEnabled = value;
                    OnPropertyChanged(nameof(IsEnabled));
                }
            }
        }

        private string _name;
        [DataMember]
        public string Name
        {
            get => _name;

            set
            {
                if (_name != value)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }
        [DataMember]
        public Scene ParentScene { get; private set; }

        [DataMember(Name = nameof(Components))]
        private ObservableCollection<Component> _components = new ObservableCollection<Component>();
        public ReadOnlyObservableCollection<Component> Components { get; private set; }

        [OnDeserialized]
        private void OnDeserialized(StreamingContext context)
        {
            if (_components != null)
            {
                Components = new ReadOnlyObservableCollection<Component>(_components);
                OnPropertyChanged(nameof(Components));
            }
        }

        public GameEntity(Scene scene)
        {
            Debug.Assert(scene != null);
            ParentScene = scene;
            _components.Add(new Transform(this));
            OnDeserialized(new StreamingContext());
        }

    }

    abstract class MSEntity : ViewModelBase
    {
        private bool _enableUpdates = true;

        private bool? _isEnabled;
        [DataMember]
        public bool? IsEnabled
        {
            get => _isEnabled;

            set
            {
                if (_isEnabled != value)
                {
                    _isEnabled = value;
                    OnPropertyChanged(nameof(IsEnabled));
                }
            }
        }

        private string _name;
        [DataMember]
        public string Name
        {
            get => _name;

            set
            {
                if (_name != value)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }

        private readonly ObservableCollection<IMSComponent> _components = new ObservableCollection<IMSComponent>();
        public ReadOnlyObservableCollection<IMSComponent> Components { get; private set; }

        public List<GameEntity> SelectedEntities { get; }


        public static float? GetMixedValue(List<GameEntity> entities, Func<GameEntity, float> getProperty)
        {
            var value = getProperty(entities.First());
            foreach(var entity in entities.Skip(1))
            {
                if (!value.IsTheSameAs(getProperty(entity)))
                {
                    return null;
                }

            }
            return value;
        }

        public static bool? GetMixedValue(List<GameEntity> entities, Func<GameEntity, bool> getProperty)
        {
            var value = getProperty(entities.First());
            foreach (var entity in entities.Skip(1))
            {
                if (value != getProperty(entity))
                {
                    return null;
                }

            }
            return value;
        }

        public static string GetMixedValue(List<GameEntity> entities, Func<GameEntity, string> getProperty)
        {
            var value = getProperty(entities.First());
            foreach (var entity in entities.Skip(1))
            {
                if (value != getProperty(entity))
                {
                    return null;
                }

            }
            return value;
        }
        protected virtual bool UpdateGameEntities(string property)
        {
            switch (property)
            {
                case nameof(IsEnabled):
                    SelectedEntities.ForEach(x=>x.IsEnabled = IsEnabled.Value); return true;

                case nameof(Name):
                    SelectedEntities.ForEach(x => x.Name = Name); return true;
            }

            return false;
        }

        protected virtual bool UpdateMSGameEntities()
        {
            IsEnabled = GetMixedValue(SelectedEntities, new Func<GameEntity, bool>(x => x.IsEnabled));
            Name = GetMixedValue(SelectedEntities, new Func<GameEntity, string>(x => x.Name));

            return true;
        }

        public void Refresh()
        {
            _enableUpdates = false;
            UpdateMSGameEntities();
            _enableUpdates = true;
        }

        public MSEntity(List<GameEntity> entities)
        {
            Debug.Assert(entities?.Any() ==  true);
            Components = new ReadOnlyObservableCollection<IMSComponent>(_components);
            SelectedEntities = entities;
            PropertyChanged += (s, e) => { if (_enableUpdates) UpdateGameEntities(e.PropertyName); };
        }
    }

    class MSGameEntity : MSEntity
    {
        public MSGameEntity(List<GameEntity> entities) : base(entities)
        {
            Refresh();
        }
    }


}
