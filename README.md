# TF2004-File-Editor

Current version: 0.2

An editor for the various proprietary file formats used in Transformers (2004) (PS2). 

This program will be able to convert model, texture, and other binary files used in Transformers to a more conventional form so they can be edited and converted back to the game's files. So far there are three file types that are understood well enough for this to be a possibility. 

A background image is supported but not currently in this repo. Any "background.png" kept in an "Assets" folder in the same directory as the exe will work.

File types:

VBIN: Most likely "Vertex Binary". The game's model files for characters. The majority can be converted to STL, but animation and texture data needs to be handled differently. Possible that .3ds or another format works better, but STL is simpler and far more portable.

VBIN.Mesh: The game's model files for levels. These are structured significantly differently from standard VBIN files and aren't well understood yet.

ITF: Might be "Infogrames Texture Format/File". The game's texture files, fairly similar to standard .BMPs. 

TMD, BMD, BDB, TDB: "Text something Defintion." "Binary Database," "Text Database." Stores a significant amount of data used by the game. The definition file contains data types and default values for their corresponding database files, and as such are required to be opened before database files and some other definition files. Currently, values in TMD and TDB files can be edited and saved as a new file to be used by the game. BMD and BDB files are still a work in progress. 

