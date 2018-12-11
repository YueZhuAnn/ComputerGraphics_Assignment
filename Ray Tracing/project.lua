-- Material
mat1 = gr.material({0.7, 0.88, 0.7}, {0, 0, 0}, 100)
mat2 = gr.material({0.9, 0.9, 0.9}, {0,0,0}, 100)
wood = gr.material({0.85, 0.48, 0.29}, {0.1, 0.1, 0.1}, 100)
mat3 = gr.material({1,1,1}, {0.1, 0.1, 0.1}, 10)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 50)
mWindowBorder = gr.material({0.44, 0.55, 0.52}, {0.05, 0.05, 0.05}, 100)
mWindowBorder2 = gr.material({0.35, 0.4, 0.37}, {0.05, 0.05, 0.05}, 100)
headbandCloth = gr.material({0.25, 0.39, 0.58}, {0.05, 0.05, 0.05}, 100)
blue = gr.material({0.0, 0.0, 0.1}, {0.1, 0.1, 0.1}, 100)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 100)
mGlass = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 50)

-- Texture
tWoodfloor = gr.texture('woodfloor.png', {0.1, 0.1, 0.1}, 25)
tWoodWall = gr.texture('woodwall.png', {0.1, 0.1, 0.1}, 25)
tSky = gr.texture('sky.png', {1.3,1.3,1.3}, 50)
tPoster = gr.texture('poster.png', {0,0,0}, 50)
tPhoto = gr.texture('Team7.png', {1.3,1.3,1.3}, 50)
tHeadbandM = gr.texture('headband.png', {1.3,1.3,1.3}, 50)
tNoodle = gr.texture('noodle.png', {0.1,0.1,0.1}, 100)
tWood1 = gr.texture('wood.png', {0.1,0.1,0.1}, 50)
tWood2 = gr.texture('wood2.png', {0.1,0.1,0.1}, 50)
tWood3 = gr.texture('wood3.png', {0.1,0.1,0.1}, 50)
tWood4 = gr.texture('wood4.png', {0.1,0.1,0.1}, 50)
tWood5 = gr.texture('wood5.png', {0.1,0.1,0.1}, 50)
tHead = gr.texture('head.png', {0.1,0.1,0.1}, 200)
tHead2 = gr.texture('head2.png', {0.1,0.1,0.1}, 200)
tMouth = gr.texture('mouth.png', {0.1,0.1,0.1}, 200)
tEar = gr.texture('ear.png', {0.1,0.1,0.1}, 200)
tEar2 = gr.texture('ear2.png', {0.1,0.1,0.1}, 200)
tBody = gr.texture('body.png', {0.1,0.1,0.1}, 200)
tBody2 = gr.texture('body2.png', {0.1,0.1,0.1}, 200)
tArm = gr.texture('arm.png', {0.1,0.1,0.1}, 200)
tArm2 = gr.texture('arm2.png', {0.1,0.1,0.1}, 200)
tFoot = gr.texture('foot.png', {0.1,0.1,0.1}, 200)
tFoot2 = gr.texture('foot2.png', {0.1,0.1,0.1}, 200)

-- Bump Mapping
bWoodfloor = gr.bumpmap('woodfloor.png')
bWoodWall = gr.bumpmap('woodwall.png')
bWall = gr.bumpmap('wall.png')
bCloth = gr.bumpmap('cloth.png')
bHeadband = gr.bumpmap('headband.png')
bNoodle = gr.bumpmap('noodle.png')
bWood = gr.bumpmap('wood.png')
bFur = gr.bumpmap('fur.png')

-- Mesh
plane = gr.mesh('plane', 'floor.obj' )

reflectPlane = gr.mesh('reflectPlane', 'floor.obj' )
reflectPlane:set_reflection(0)

windowBorder = gr.mesh('windowBorder', 'window.obj' )

mirrorBorder = gr.mesh('mirrorBorder', 'border.obj' )
mirrorPlane = gr.mesh('mirrorPlane', 'mirror.obj' )
mirrorPlane:set_reflection(0)

sky = gr.nh_sphere('sky', {-50,150,100}, 500)

box = gr.nh_box('box', {0,0,0},1)

glassbox = gr.nh_box('box', {0,0,0},1)
glassbox:set_refraction(1, 0.1)

cylinder = gr.nh_cylinder('cylinder', {0,0,0},1,1)

cup = gr.mesh('cup', 'cup.obj' )
cup:set_refraction(1.5, 0)
water =gr.mesh('water', 'water.obj' )
water:set_refraction(1.33, 0)

cabinet = gr.mesh('cabinet', 'cabinet.obj')
cabinet2 = gr.mesh('cabinet2', 'cabinet2.obj')
cabinet3 = gr.mesh('cabinet3', 'cabinet3.obj')
cabinetglass = gr.mesh('cabinetglass', 'cabinetglass.obj')
cabinetglass:set_refraction(1,0.1)
cabinetglass:set_reflection(0.6)

closet = gr.mesh('closet', 'closet.obj')
closet1 = gr.mesh('closet1', 'closet1.obj')
closet2 = gr.mesh('closet2', 'closet2.obj')

photoBorder = gr.mesh('photoBorder', 'photoborder.obj')
photo = gr.mesh('photo', 'photo.obj')
photoglass = gr.mesh('photoglass', 'photoglass.obj')
photoglass:set_refraction(1,0.1)
photoglass:set_reflection(0.9)

noodle = gr.mesh('noodle', 'noodle.obj')
chopsticks = gr.mesh('chopsticks', 'chopsticks.obj')

headbandM = gr.mesh('headbandM', 'headband.obj')
headbandC = gr.mesh('headbandC', 'headbandc.obj')

bearHead = gr.mesh('bearHead', 'head.obj')
bearEar = gr.mesh('bearEar', 'ear.obj')
bearEye = gr.mesh('bearEye', 'eye.obj')
bearBody = gr.mesh('bearBody', 'body.obj')
bearArm = gr.mesh('bearArm', 'arm.obj')
bearArm2 = gr.mesh('bearArm', 'arm2.obj')
bearFoot = gr.mesh('bearFoot', 'foot.obj')
bearMouth = gr.mesh('bearMouth', 'mouth.obj')
bearNose = gr.mesh('bearNose', 'nose.obj')

-- Scene Start
scene = gr.node('scene')

-- Sky
scene:add_child(sky, mat2, tSky, nil)

-- Floor
floor = gr.node('floor')
floor:scale(70, 40, 30)
floor:translate(30, 0, 0)
floor:add_child(plane, mat2, tWoodfloor, bWoodfloor)
scene:add_child(floor)

-- Ceiling
ceiling = gr.node('ceiling')
ceiling:scale(70, 40, 30)
ceiling:translate(30, 70, 0)
ceiling:add_child(plane, mat1, nil, bWall)
scene:add_child(ceiling)

-- Wall
wallBottom  = gr.node('wallBottom')

wallBottom1 = gr.node('wallBottom1')
wallBottom1:scale(50,10,0.5)
wallBottom1:add_child(box, hide, tWoodWall, bWoodWall)
wallBottom:add_child(wallBottom1)

wallBottom2 = gr.node('wallBottom1')
wallBottom2:scale(50,0.9,0.5)
wallBottom2:translate(0,3,0.15)
wallBottom2:add_child(box, wood, nil, nil)
wallBottom:add_child(wallBottom2)

wallBottom3 = gr.node('wallBottom3')
wallBottom3:rotate('Z', -90)
wallBottom3:scale(50,0.3,0.3)
wallBottom3:translate(0,10.3,0.26)
wallBottom3:add_child(cylinder, wood, nil, nil)
wallBottom:add_child(wallBottom3)

wall1 = gr.node('wall1')
wall1:scale(70, 30, 35)
wall1:rotate('X', 90)
wall1:translate(30,35,-30)
wall1:add_child(plane,mat1,nil,bWall)
scene:add_child(wall1)

wall1_1 = gr.node('wall_1')
wall1_1:add_child(wallBottom)
wall1_1:scale(3, 3, 3)
wall1_1:translate(-40,-10,-31)
scene:add_child(wall1_1)

wall21 = gr.node('wall21')
--wall21:rotate('Z', 180)
wall21:scale(10, 30, 30)
wall21:rotate('Z', 90)
wall21:translate(-40,60,0)
wall21:add_child(plane,mat1,nil,bWall)
scene:add_child(wall21)

wall22 = gr.node('wall22')
--wall22:rotate('Z', 180)
wall22:scale(10, 30, 30)
wall22:rotate('Z', 90)
wall22:translate(-40,10,0)
wall22:add_child(plane,mat1,nil,bWall)
scene:add_child(wall22)

wall23 = gr.node('wall23')
--wall23:rotate('Z', 180)
wall23:scale(35, 30, 10)
wall23:rotate('Z', 90)
wall23:translate(-40,35,-30)
wall23:add_child(plane,mat1,nil,bWall)
scene:add_child(wall23)

wall24 = gr.node('wall24')
--wall24:rotate('Z', 180)
wall24:scale(35, 30, 5)
wall24:rotate('Z', 90)
wall24:translate(-40,35,25)
wall24:add_child(plane,mat1,nil,bWall)
scene:add_child(wall24)

wall2_1 = gr.node('wal2_1')
wall2_1:scale(0.5,1,1)
wall2_1:add_child(wallBottom)
wall2_1:rotate('Y', 90)
wall2_1:scale(3, 3, 2.5)
wall2_1:translate(-41,-10,30)
scene:add_child(wall2_1)

wall3 = gr.node('wall3')
--wall3:rotate('Z', 180)
wall3:scale(35, 30, 30)
wall3:rotate('Z', -90)
wall3:translate(90,35,0)
wall3:add_child(plane,mat1,nil,bWall)
scene:add_child(wall3)

wall3_1 = gr.node('wal3_1')
wall3_1:add_child(wallBottom)
wall3_1:scale(0.5,1,1)
wall3_1:rotate('Y', -90)
wall3_1:scale(3, 3, 2.5)
wall3_1:translate(91,-10,-30)
scene:add_child(wall3_1)

wall4 = gr.node('wall4')
wall4:scale(70, 30, 35)
--wall4:rotate('Z', 180)
wall4:rotate('X', 90)
wall4:translate(30,35,30)
wall4:add_child(plane,mat1,nil,bWall)
scene:add_child(wall4)

wall4_1 = gr.node('wal4_1')
wall4_1:add_child(wallBottom)
wall4_1:rotate('Y', -180)
wall4_1:scale(3, 3, 3)
wall4_1:translate(90,-10,30)
scene:add_child(wall4_1)

-- Window
window = gr.node('window')
window:rotate('Z', 90)
window:translate(-38,36,0)
scene:add_child(window)

windowBorderNode = gr.node('windowBorderNode')
windowBorderNode:scale(16.5,3,20)
windowBorderNode:translate(1,0,0)
window:add_child(windowBorderNode)
windowBorderNode:add_child(windowBorder, mWindowBorder, nil, nil)

windowL = gr.node('windowl')
window:add_child(windowL)
windowL:translate(1.1,0.6,8)

windowR = gr.node('windowr')
window:add_child(windowR)
windowR:translate(1.1,1,-5)

mywindowl = gr.node('mywindowl')
windowL:add_child(mywindowl)
mywindowl:scale(1,1.1,1.3)

mywindowr = gr.node('mywindowr')
windowR:add_child(mywindowr)
mywindowr:scale(1,1.1,1.3)

nGlass = gr.node('nGlass')
nGlass:scale(24.2,1.2,12)
nGlass:translate(-12,-0.5,-5.8)
nGlass:add_child(glassbox, mat2, nil, nil)
mywindowl:add_child(nGlass)
mywindowr:add_child(nGlass)

nWindowborder2 = gr.node('nWindowborder2')
nWindowborder2:scale(14,1,7)
nWindowborder2:add_child(windowBorder, mWindowBorder2, nil, nil)
mywindowl:add_child(nWindowborder2)
mywindowr:add_child(nWindowborder2)

-- Furniture
mirror = gr.node('mirror')
mirror:scale(8,1,9)
mirror:rotate('X', 87)
mirror:translate(0,23,-28)
mirror:add_child(mirrorPlane, mat1, nil, nil)
mirror:add_child(mirrorBorder, wood, nil, bWood)
scene:add_child(mirror)

-- Cabinet
nCabinet = gr.node('nCabinet')
nCabinet:rotate('Y', -90)
nCabinet:scale(5.5,5,1)
nCabinet:translate(37,12.5,-25)
nCabinet:add_child(cabinet, wood, tWood3, nil)
nCabinet:add_child(cabinet2, wood, tWood2, nil)
nCabinet:add_child(cabinet3, wood, tWood5, nil)
nCabinet:add_child(cabinetglass, wood, nil, nil)
scene:add_child(nCabinet)

-- Closet
nCloset = gr.node('nCloset')
nCloset:scale(7.5,8,7.5)
nCloset:rotate('Y', -90)
nCloset:translate(-22,25,28)
nCloset:add_child(closet1, wood, tWood1, nil)
nCloset:add_child(closet2, wood, tWood2, nil)
scene:add_child(nCloset)

-- Poster
nPoster = gr.node('nPoster')
nPoster:scale(10,1,15)
nPoster:rotate('Y', 180)
nPoster:rotate('X', 90)
nPoster:translate(37,46,-29.2)
nPoster:add_child(plane, wood, tPoster, bCloth)
scene:add_child(nPoster)

-- Photo
nPhoto = gr.node('nPhoto')
nPhoto:scale(2,2,1.5)
nPhoto:rotate('Z', 20)
nPhoto:rotate('Y', 50)
nPhoto:translate(50, 27.5, -25.5)
nPhoto:add_child(photoBorder, wood, tWood4, bWood4)
nPhoto:add_child(photo, wood, tPhoto, nil)
nPhoto:add_child(photoglass, wood, nil, nil)
scene:add_child(nPhoto)

-- Headband
nHeadband = gr.node('nHeadband')
nHeadband:rotate('Y', -90)
nHeadband:scale(1.5, 0.5, 0.5)
nHeadband:translate(40, 10, -25)
nHeadband:add_child(headbandM, wood, tHeadbandM, bHeadband)
nHeadband:add_child(headbandC, headbandCloth, nil,nil)
scene:add_child(nHeadband)

-- Cup of water
nCupWater = gr.node('nCupWater')
nCupWater:scale(1.65,1.98,1.65)
nCupWater:translate(35, 25.5, -24)
scene:add_child(nCupWater)

nCup = gr.node('nCup')
nCup:add_child(cup, mGlass, nil, nil)
nCupWater:add_child(nCup)

nWater = gr.node('nWater')
nWater:add_child(water, mGlass, nil, nil)
nCupWater:add_child(nWater)

-- Noodle
nNoodle = gr.node('nNoodle')
nNoodle:rotate('Z', 180)
nNoodle:rotate('Y', -20)
nNoodle:scale(2.2,3.5,2.2)
nNoodle:translate(42, 28, -26)
nNoodle:add_child(noodle, wood, tNoodle, nil)
nNoodle:add_child(chopsticks, wood, tWood1, bWood)
scene:add_child(nNoodle)

-- Bear
nBear = gr.node('nBear')
nBear:translate(0,0,-0.5)
scene:add_child(nBear)

nBear1 = gr.node('nBear1')
nBear1:rotate('Z', 180)
nBear1:rotate('Y', 110)
nBear1:scale(2,2,2)
nBear1:translate(18, 25, -25)
nBear1:add_child(bearHead, wood, tHead, bFur)
nBear1:add_child(bearEar, wood, tEar, bFur)
nBear1:add_child(bearEye, blue, nil, nil)
nBear1:add_child(bearBody, wood, tBody, bFur)
nBear1:add_child(bearArm, wood, tArm, bFur)
nBear1:add_child(bearFoot, wood, tFoot, bFur)
nBear1:add_child(bearMouth, wood, tMouth, bFur)
nBear1:add_child(bearNose, black, nil, nil)
nBear:add_child(nBear1)

nBear2 = gr.node('nBear2')
nBear2:rotate('Z', 180)
nBear2:rotate('Y', 110)
nBear2:scale(2,2,2)
nBear2:translate(23, 25, -26.5)
nBear2:add_child(bearHead, wood, tHead2, bFur)
nBear2:add_child(bearEar, wood, tEar2, bFur)
nBear2:add_child(bearEye, blue, nil, nil)
nBear2:add_child(bearBody, wood, tBody2, bFur)
nBear2:add_child(bearArm2, wood, tArm2, bFur)
nBear2:add_child(bearMouth, wood, tMouth, bFur)
nBear2:add_child(bearMouth, wood, tMouth, nil)
nBear2:add_child(bearNose, black, nil, nil)
nBear:add_child(nBear2)

white_light = gr.light({30, 65, 0}, {0.8,0.8,0.8}, {1, 0.005, 0}, 5, 0.5)
orange_light = gr.light({-100,150,100}, {0.93,0.9,0.9}, {1, 0.023, 0}, 8, 0)

focalLen = gr.focal(20,10,0.05)

gr.render(scene, 'project.png',1280, 720,
	  {20, 35, -5}, {0, 0, -1}, {0, 1, 0}, 105,
	  {0.3, 0.3, 0.3}, {white_light,orange_light}, focalLen)