#version 440
in vec2 fsTexCoord;
out vec4 color;

uniform sampler2D screenTexture;
uniform sampler2D pigmentTexture;
uniform sampler2D turbulenceTexture;
uniform sampler2D paperTexture;

uniform float pixelWidth;
uniform float pixelHeight;

uniform float offsetX; 
uniform float offsetY; 
uniform float brushThreshold;
uniform vec3 brushColor;

uniform float scaleFactorPigment;
uniform float scaleFactorTurbulence;
uniform float scaleFactorPaper;


uniform bool enableBrush, enableWobble, enableEdgeDarkening, enablePigment, enableTurbulence, enablePaper;
void main()
{
        vec2 offsets[9] = vec2[](
        vec2(-offsetX, offsetY),  // top-left
        vec2(0.0f,    offsetY),  // top-center
        vec2(offsetX,  offsetY),  // top-right
        vec2(-offsetX, 0.0f),    // center-left
        vec2(0.0f,    0.0f),    // center-center
        vec2(offsetX,  0.0f),    // center-right
        vec2(-offsetX, -offsetY), // bottom-left
        vec2(0.0f,    -offsetY), // bottom-center
        vec2(offsetX,  -offsetY)  // bottom-right    
    );

    vec3 col;

    vec3 abstractedColor = vec3(texture(screenTexture, fsTexCoord.st));
    float paperIntensity = texture(paperTexture, fsTexCoord.st).r;
    col = abstractedColor;

    //wobbling effect
    //get the x and y gradients; shift current color left-right, up-down based on the gradient
    float xGradLeft = texture(paperTexture, fsTexCoord.st + offsets[3]).r; // center left
    float xGradRight = texture(paperTexture, fsTexCoord.st + offsets[5]).r; // center left    
    float xGradient = xGradLeft - xGradRight;
    float yGradTop = texture(paperTexture, fsTexCoord.st + offsets[1]).r; // center left
    float yGradBot = texture(paperTexture, fsTexCoord.st + offsets[7]).r; // center left
    float yGradient = yGradTop - yGradBot;
    if(enableWobble)
    {
        // float xx, yy;
        // if(xGradient > 0.0)
        //     xx = 0.0;
        // else
        //     xx = xGradient;

        // if(yGradient > 0.0)
        //     yy = 0.0; 
        // else
        //     yy = yGradient;
        col = vec3(texture(screenTexture, 
            vec2(fsTexCoord.x - xGradient * offsetX, 
                fsTexCoord.y - yGradient * offsetY)));
    }
    else
    {

       if(xGradient > 0.0)
            col = (col + vec3(texture(screenTexture, fsTexCoord.st + offsets[3]))) * 0.5;
        else
            col = (col + vec3(texture(screenTexture, fsTexCoord.st + offsets[5]))) * 0.5;

      if(yGradient > 0.0)
            col = (col + vec3(texture(screenTexture, fsTexCoord.st + offsets[1]))) * 0.5;
        else
            col = (col + vec3(texture(screenTexture, fsTexCoord.st + offsets[7]))) * 0.5;
    }

    //dry brush effect
    //apply a simple treshold on the the paper intensity
    if(enableBrush && paperIntensity > brushThreshold)
        col = col + vec3(0.2);
 
    if(enableEdgeDarkening)
    {        

        //edge darkening
        // ∆(Px,y) = |Px−1,y − Px+1,y|+|Px,y−1 − Px,y+1|
        //The gradient intensity (used to modify the pigment density using formula 1) 
        //is computed by averaging the gradient of each color channel.
        vec3 colDeltaX_RGB = vec3(texture(screenTexture, fsTexCoord.st + 
            vec2(-pixelWidth, 0))) - vec3(texture(screenTexture, fsTexCoord.st + vec2(pixelWidth, 0)));
        float colDeltaX_Gray = abs((colDeltaX_RGB.r + colDeltaX_RGB.g + colDeltaX_RGB.b) * 0.3333);
        vec3 colDeltaY_RGB = vec3(texture(screenTexture, fsTexCoord.st + 
            vec2(0, -pixelHeight))) - vec3(texture(screenTexture, fsTexCoord.st + vec2(0, pixelHeight)));
        float colDeltaY_Gray = abs((colDeltaY_RGB.r + colDeltaY_RGB.g + colDeltaY_RGB.b) * 0.3333);
        col = col - (1 - vec3(col*col)) * (colDeltaX_Gray + colDeltaY_Gray);
    }
    if(enablePigment)
    {
        //pigment disperion layer
        float pigmentDensity = 1.0 + scaleFactorPigment
         * (texture(pigmentTexture, fsTexCoord).r - 0.5);
        col = col - (col - vec3(col*col)) * (pigmentDensity - 1.0);
    }
    if(enableTurbulence)
    {
        float turbulenceDensity = 1 + scaleFactorTurbulence
         * (texture(turbulenceTexture, fsTexCoord).r - 0.5);
        col = col - (col - vec3(col*col)) * (turbulenceDensity - 1.0);
        //col = col * -turbulenceDensity;
        // col = col - (1 - vec3(col*col)) * (turbulenceDensity - 1.0);
    // //col = col*(vec3(1.0)-(vec3(1.0)-col) * vec3(turbulenceDensity-1.0));  
    }
    if(enablePaper)
    {
        float paperDensity = 1 + scaleFactorPaper * 
        (texture(paperTexture, fsTexCoord).r - 1.0);
        col = col - (col - vec3(col*col)) * (paperDensity - 1.0);
        // //col = col*(vec3(1.0)-(vec3(1.0)-col) * vec3(paperDensity-1.0));        
    }
    color = vec4(col, 1.0);
}  