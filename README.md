#Recomposed-platformer

A reconstruction of some files laying around named just "plataformer". I found those in my website when doing cleaning up chores and just had to get them running.

##Building this stuff

The "tools" project has been hacked into the project to simulate its state whitin the original period when this was written.

The original source has been hacked out of its mind too, so there might be inconsistencies and strange code dependencies.

- Get libdansdl2 in the classic branch and make it.
- Get the capa_compatibilidad_libdansdl2 thing. There's no need to make that one.
- Go into tools/ and set the correct paths.
- Make the project in tools.
- Copy the resulting objects from tools/objects to /objects.
- Set the correct paths in the makefile_linux file. Make sure you point at the right "tools".
- Make the project.
