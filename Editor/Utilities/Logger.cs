﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Interop;
using System.Windows.Shapes;

namespace Editor.Utilities
{

    enum MessageType
    {
        Information = 0x01,
        Warning = 0x02,
        Error = 0x04,
    }

    class LogMessage
    {
        public DateTime Time { get; }
        public MessageType MessageType { get; }
        public string Message { get; }
        public string File { get; }
        public string Caller { get; }
        public int Line { get; }
        public string MetaData => $"{File}: {Caller} (l.{Line})";

        public LogMessage(MessageType type, string msg, string file, string caller, int line)
        {
            Time = DateTime.Now;
            MessageType = type;
            Message = msg;
            File = System.IO.Path.GetFileName(file);
            Caller = caller;
            Line = line;
        }
    }

    static class Logger
    {
        private static int _messageFilter = (int)(MessageType.Information | MessageType.Warning | MessageType.Error);

        private static readonly ObservableCollection<LogMessage> _messages = new ObservableCollection<LogMessage>();
        public static ReadOnlyCollection<LogMessage> Messages { get; } = new ReadOnlyCollection<LogMessage>(_messages);

        public static CollectionViewSource FilteredMessages { get; } = new CollectionViewSource() { Source = Messages };

        public static async void Log( MessageType type, string msg, [CallerFilePath]string file="", [CallerMemberName]string caller="", [CallerLineNumber]int line = 0)
        {
            await Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                _messages.Add(new LogMessage(type, msg, file, caller, line));
                FilteredMessages.View.Refresh();
            }));
        }

        public static async void Clear()
        {
            await Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                _messages.Clear();
                FilteredMessages.View.Refresh();
            }));
        }

        public static void SetMessageFiltered(int mask)
        {
            _messageFilter = mask;
            FilteredMessages.View.Refresh();
        }
        static Logger()
        {
            FilteredMessages.Filter += (s, e) =>
            {
                var type = (int)(e.Item as LogMessage).MessageType;
                e.Accepted = (type & _messageFilter) != 0;
            };
        }
    }
}
