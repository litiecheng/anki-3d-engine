# Copyright (C) 2009-2016, Panagiotis Christopoulos Charitos and contributors.
# All rights reserved.
# Code licensed under the BSD License.
# http://www.anki3d.org/LICENSE

# keep methods in alphabetical order

# blender imports
import bpy

bl_info = {"author": "A. A. Kalugin Jr."}

def get_blender_images():
	"""
	Gets the blender images using the materials
	"""
	bl_images = []
	mats = bpy.data.materials
	for mat in mats:
		for slot in mat.texture_slots:
			if slot:
				if (slot.texture.image != None):
					bl_images.append(slot.texture.image.file_format)
	return bl_images

