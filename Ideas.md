# Ideas

 - Have IAssetBundle "record" loading calls and keep track; can create dependency tree. Then when any of the "dependencies" update, any asset that depended on another to load will reload.