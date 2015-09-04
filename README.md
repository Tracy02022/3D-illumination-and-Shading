# 3D-illumination-and-Shading
Using a little bunny model to implement 3D illumination and Shading 
Goal –The goal is to give you some first-hand experiences of triangle/polygon based 3D modeling, 3D transformation, as well as mouse/keyboard interaction with 3D objects..
Important Steps:
(1) Correct computation on cross-product between vectors
(2) Correct computation on view, light, and normal vectors 
(3) Correct illumination model for the ambient term 
(4) Correct illumination model for the diffuse term 
(5) Correct illumination model for the phong term
(6) Correct keyboard callback function to switch between different views 
  •	hit “0” to show the bunny with no shading at all (a single color for all the triangles).
  •	hit “1” to show the bunny with only diffuse and ambient terms. The result looks like a plaster material.
  •	hit “2” to show the bunny with diffuse, ambient, and phong terms. The result may look like different types of materials,
  depending on your choice of the exponential p for the phong terms, e.g. china material (p = [55, 100],
  metal material p = [0.1, 1.0]).
  •	hit key “M” or “m" only to show the mesh lines (no triangle surface). 
  
