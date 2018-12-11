-- puppet.lua

rootnode = gr.node('root')

white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
skin = gr.material({255.0/255.0, 108.0/255.0, 0.0/255.0}, {0.05, 0.05, 0.05}, 20)

toBodyjoint = gr.joint('toBody', {-10/180,0,10/180}, {0,0,0})
rootnode:add_child(toBodyjoint)

body = gr.mesh('body', 'Body', true)
toBodyjoint:add_child(body)
body:set_material(skin)
body:scale(0.5,0.5,0.5)
body:rotate('y', -180)

Body2 = gr.mesh('body2', 'Body2', false)
body:add_child(Body2)
Body2:set_material(white)
Body2:scale(0.51,0.51,0.51)

-- Right front leg
toRF1joint = gr.joint('toRF1', {0,0.25,60/180}, {0,0,0})
toRF1joint:translate(0.2,0,-0.5)
body:add_child(toRF1joint)

RF1 = gr.mesh('arm', 'RightFrontThigh', true)
toRF1joint:add_child(RF1)
RF1:set_material(skin)
RF1:scale(0.15,0.15,0.2)
RF1:translate(0,-0.3,0)

toRF2joint = gr.joint('toRF2', {-0.5,-60/180,0}, {0,0,0})
toRF2joint:translate(0,-0.2,0)
RF1:add_child(toRF2joint)


RF2 = gr.mesh('arm', 'RightFrontCalf', true)
toRF2joint:add_child(RF2)
RF2:set_material(skin)
RF2:scale(0.08,0.08,0.15)
RF2:translate(0,-0.14,0.0)

toRF3joint = gr.joint('toRF3', {-0.5,-60/180,0}, {0,0,0})
toRF3joint:translate(0,-0.1,0.0)
RF2:add_child(toRF3joint)

RF3 = gr.mesh('foot', 'RightFrontFoot', true)
toRF3joint:add_child(RF3)
RF3:set_material(white)
RF3:scale(0.08,0.08,0.08)
RF3:rotate('y', 90)

-- Left front leg
toLF1joint = gr.joint('toLF1', {0,0,60/180}, {0,0,0})
toLF1joint:translate(-0.2,0,-0.5)
body:add_child(toLF1joint)

LF1 = gr.mesh('arm', 'LeftFrontThigh', true)
toLF1joint:add_child(LF1)
LF1:set_material(skin)
LF1:scale(0.15,0.15,0.2)
LF1:translate(0,-0.3,0)

toLF2joint = gr.joint('toLF2',  {-0.5,0,0}, {0,0,0})
toLF2joint:translate(0,-0.2,0)
LF1:add_child(toLF2joint)

LF2 = gr.mesh('arm', 'LeftFrontCalf', true)
toLF2joint:add_child(LF2)
LF2:set_material(skin)
LF2:scale(0.08,0.08,0.15)
LF2:translate(0,-0.14,0.0)

toLF3joint = gr.joint('toLF3', {-0.5,0,0}, {0,0,0})
toLF3joint:translate(0,-0.1,0.0)
LF2:add_child(toLF3joint)

LF3 = gr.mesh('foot', 'LeftFrontFoot', true)
toLF3joint:add_child(LF3)
LF3:set_material(white)
LF3:scale(0.08,0.08,0.08)
LF3:rotate('y', 90)

-- Right back leg
toRB1joint = gr.joint('toRB1', {-10/180,0,50/180}, {0,0,0})
toRB1joint:translate(0.05,-0.2,0.5)
body:add_child(toRB1joint)


RB1 = gr.mesh('arm', 'RightBackThigh', true)
toRB1joint:add_child(RB1)
RB1:set_material(skin)
RB1:scale(0.15,0.2,0.2)
RB1:translate(0.15,-0.3,0)

toRB2joint = gr.joint('toRB2', {-0.5,0,0}, {0,0,0})
toRB2joint:translate(0,-0.3,0)
RB1:add_child(toRB2joint)

RB2 = gr.mesh('arm', 'RightBackCalf', true)
toRB2joint:add_child(RB2)
RB2:set_material(skin)
RB2:scale(0.08,0.08,0.15)
RB2:translate(0,-0.1,0)

toRB3joint = gr.joint('toRB3', {-0.5,0,0}, {0,0,0})
toRB3joint:translate(0,-0.1,0)
RB2:add_child(toRB3joint)

RB3 = gr.mesh('foot', 'RightBackFoot', true)
toRB3joint:add_child(RB3)
RB3:set_material(white)
RB3:scale(0.08,0.08,0.08)
RB3:rotate('y', 90)

-- Left back leg

toLB1joint = gr.joint('toLB1',{-10/180,30/180,50/180}, {0,0,0})
toLB1joint:translate(-0.05,-0.2,0.5)
body:add_child(toLB1joint)

LB1 = gr.mesh('arm', 'LeftBackThigh', true)
toLB1joint:add_child(LB1)
LB1:set_material(skin)
LB1:scale(0.15,0.2,0.2)
LB1:translate(-0.15,-0.30,0)

toLB2joint = gr.joint('toLB2', {-0.5,-60/180,0}, {0,0,0})
toLB2joint:translate(0,-0.3,0)
LB1:add_child(toLB2joint)

LB2 = gr.mesh('arm', 'LeftBackCalf', true)
toLB2joint:add_child(LB2)
LB2:set_material(skin)
LB2:scale(0.08,0.08,0.15)
LB2:translate(0,-0.1,0)

toLB3joint = gr.joint('toLB3', {-0.5,-60/180,0}, {0,0,0})
toLB3joint:translate(0,-0.1,0)
LB2:add_child(toLB3joint)

LB3 = gr.mesh('foot', 'LeftBackFoot', true)
toLB3joint:add_child(LB3)
LB3:set_material(white)
LB3:scale(0.08,0.08,0.08)
LB3:rotate('y', 90)


-- head
toHeadjoint = gr.joint('toHead', {-0.15,0,0.25}, {-30/180,0,30/180})
toHeadjoint:translate(0,0.3,-0.65)
body:add_child(toHeadjoint)

Head = gr.mesh('head', 'Head', true)
toHeadjoint:add_child(Head)
Head:set_material(skin)
Head:scale(0.55,0.55,0.55)
Head:translate(0.2,0.15,0)
Head:rotate('y', 90)

Head2 = gr.mesh('head2', 'Head2', false)
Head:add_child(Head2)
Head2:set_material(white)
Head2:scale(0.56,0.56,0.56)

Nose = gr.mesh('nose', 'Nose', false)
Head:add_child(Nose)
Nose:set_material(black)
Nose:scale(0.55,0.55,0.55)

Eye = gr.mesh('eye', 'Eye', false)
Head:add_child(Eye)
Eye:set_material(black)
Eye:scale(0.57,0.57,0.57)

toEarRjoint = gr.joint('toEarR', {20/180,20/180,50/180}, {-10/180,0,10/180})
toEarRjoint:translate(0.05,0.2,0.25)
Head:add_child(toEarRjoint)


EarR = gr.mesh('ear', 'Right Ear', true)
toEarRjoint:add_child(EarR)
EarR:set_material(skin)
EarR:scale(0.9,0.9,0.9)
EarR:translate(0,0.05,-0.05)

EarR2 = gr.mesh('ear2', 'EarR2', false)
EarR:add_child(EarR2)
EarR2:set_material(white)
EarR2:scale(0.91,0.91,0.91)

toEarLjoint = gr.joint('toEarL', {-50/180,-20/180,-20/180}, {170/180,1,190/180})
toEarLjoint:translate(0.05,0.2,-0.20)
Head:add_child(toEarLjoint)

EarL = gr.mesh('ear', 'Left Ear', true)
toEarLjoint:add_child(EarL)
EarL:set_material(skin)
EarL:scale(0.9,0.9,0.9)
EarL:translate(0,0.05,-0.05)

EarL2 = gr.mesh('ear2', 'EarL2', false)
EarL:add_child(EarL2)
EarL2:set_material(white)
EarL2:scale(0.91,0.91,0.91)

-- tail
toTail1joint = gr.joint('toTail1', {-150/180,-60/180,-45/180}, {0,0,0})
toTail1joint:translate(0,-0.15,0.5)
body:add_child(toTail1joint)

Tail1 = gr.mesh('tail1', 'Tail', true)
toTail1joint:add_child(Tail1)
Tail1:set_material(skin)
Tail1:scale(0.5,0.5,0.5)
Tail1:translate(0,-0.4,-0.25)

Tail2 = gr.mesh('tail2', 'Tail2', true)
Tail1:add_child(Tail2)
Tail2:set_material(white)
Tail2:scale(0.51,0.51,0.51)

return rootnode
