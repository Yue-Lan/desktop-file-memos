# 文件便笺
这是一个Linux上的桌面分类应用，它提供了一个托盘图标进行便笺添加，每个便笺都由标题栏和文件视图构成，标题栏有一个show/hide按钮，提供一个右键菜单，文件视图支持双击打开和右键菜单操作。

截图见 [README.md](https://github.com/Yue-Lan/desktop-file-memos/blob/master/README.md)

# 思路
- 使用Qt的Model/View开发模式，基于QFileSystemModel，QListView和Delegate进行一些定制，实现便笺的主体功能；
- 一下Qt难以实现的功能，使用Gio实现，最典型的一个例子——使用gvfs提供的回收站；
- 使用QSettings保存窗口的状态，以便关闭后恢复；每一个便笺显示的都是一个隐藏的目录中的内容，每一个目录对应唯一的id，并以id命名，窗口的状态也由这个id为索引；


- 关于图标和主题

  <br>文件的图标基于QFileIconProvider的icon方法进行定制，主要用到的是QIcon的formTheme方法；</br>
  <font color = red size = 20>为了适应gtk的主题，我们需要安装qt5-gtk-platformtheme这一个包，在运行时传入参数“-platformtheme gtk3”。</font>

- 关于安装，有一个debian分支，可以使用dpkg-buildpackage进行打包，如果只是测试，只需要使用qmake或者qtcreator即可。如果不想进行打包安装，可以参见README的install方法。
  
