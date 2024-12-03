**Extended View Classes**

This folder includes a utility subclass for each QView and QWidget (like QTreeView and QTreeWidget). 
The primary purpose of these classes is to manage key press event logic from outside the view or 
widget class without the need of subclass each different one.
 
By emitting the `keyPressed` signal whenever a key is pressed, the event can be handled externally, 
allowing to assign specific behaviors (e.g., handling the Delete key) without the need to subclass 
each individual view or widget.