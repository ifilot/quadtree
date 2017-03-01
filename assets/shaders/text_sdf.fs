/****************************************************************************
#                                                                           #
#    INORGANIC MATERIALS CHEMISTRY CONFIDENTIAL                             #
#                                                                           #
#    Copyright 2016 Inorganic Materials Chemistry                           #
#                   Eindhoven University of Technology (TU/e)               #
#                                                                           #
#    All Rights Reserved.                                                   #
#                                                                           #
#    NOTICE:  All information contained herein is, and remains              #
#    the property of Inorganic Materials Chemistry (TU/e) and its suppliers,#
#    if any.  The intellectual and technical concepts contained             #
#    herein are proprietary to Inorganic Materials Chemistry (TU/e)         #
#    and its suppliers and may be covered by U.S. and Foreign Patents,      #
#    patents in process, and are protected by trade secret or copyright law.#
#    Dissemination of this information or reproduction of this Materials    #
#    is strictly forbidden unless prior written permission is obtained      #
#    from Inorganic Materials Chemistry (TU/e).                             #
#                                                                           #
#    Authors: Ivo Filot       <i.a.w.filot@tue.nl>                          #
#             Emiel Hensen    <e.j.m.hensen@tue.nl>                         #
#                                                                           #
*****************************************************************************/

#version 330 core

in vec2 texcoord;
out vec4 color;

uniform sampler2D text;
uniform vec3 textcolor;
uniform float width;
uniform float edge;

void main() {
    float distance = 1.0 - texture(text, texcoord).r;
    float alpha = 1.0 - smoothstep(width, width+edge, distance);

    color = vec4(textcolor, alpha);
}
