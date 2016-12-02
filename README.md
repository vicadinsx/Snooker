# README #

relevant for shader to have a uniform with the lighting info?

each object must have a physical representation associated with its visual representation.
(ex: sphere associated with billard ball mesh)

physics engine holds a list of physical representations of objects, which is in turn linked to a list of visual representations on the visual engine

* on each update
    * check elapsed seconds
    * update
        * physics engine (feeds onto the visual engine)
        * visual engine
* for each ball in list of balls
    * see if it colides with any of hte other balls
        * if so, create collision objects
* after checking all balls, resolve all collision objects
* (repeat until no more collision objects are generated)

move onto the visual engine -> draw scenegraph
	note: need to update the model matrix of each ball to match collisions
	
### post-processing ### 

instead of rendering to screen, take framebuffer and render it onto a texture. 
map that texture onto a square, that will be rendered onto the screen
before doing so, edit that texture with the shader of the effect.