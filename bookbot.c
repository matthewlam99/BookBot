//
//  bookbot.c
//  
//
//  Created by Matthew Lam, June Park, Michael Chen, Felix Wong on 2018-12-03.
//


/*
 S1 - Colour Sensor
 S2 - Touch Sensor to Stop System
 S3 - Ultra
 S4 - Touch Sensor on Claw
 
 Motors
 A - Drive motor
 B - Arm
 C - Claw
 D - Drive Motor
 */

int const CLAMP = 50;
bool const SHUT = true;

bool storage[2][4];
float const storeDist[2][4] = {{0, 120, 360, 580},
    {800, 1000, 1200, 1400}};

void moveBwd();
void clawClose(bool noGrip);
void armRetract();

void returnPosition()
{
    displayBigTextLine(3, "Release");
    wait1Msec(2000);
    clawClose(SHUT);
    armRetract();
    moveBwd();
    eraseDisplay();
}

bool isNotAbort()
{
    if(SensorValue[S2] == 1)
    {
        displayBigTextLine(3, "ABORTING!!!");
        return false;
    }
    return true;
}

void moveBwd()
{
    motor[motorA] = motor[motorD] = -80;
    while(SensorValue[S3] > 5 && isNotAbort())
    {}
    motor[motorA] = motor[motorD] = 0;
}

void moveFwd(int encoder_lim)
{
    nMotorEncoder[motorA] = 0;
    motor[motorA] = motor[motorD] = 80;
    while (fabs(nMotorEncoder[motorA]) < encoder_lim && isNotAbort())
    {}
    motor[motorA] = motor[motorD] = 0;
}

void armExtend()
{
    time1[T3] = 0;
    motor[motorB] = 100;
    while(time1[T3] < 2500 && isNotAbort())
    {}
    motor[motorB] = 0;
}

void armRetract()
{
    time1[T3] = 0;
    motor[motorB] = -100;
    while(time1[T3] < 2500 && isNotAbort())
    {}
    motor[motorB] = 0;
}

void clawClose(bool notGrip)
{
    time1[T4] = 0;
    motor[motorC] = 60;
    while(time1[T4] < 500 && isNotAbort())
    {}
    if(notGrip)
        motor[motorC] = 0;
}

void clawOpen(float CLAMP)
{
    nMotorEncoder[motorC] = 0;
    motor[motorC] = -25;
    while(fabs(nMotorEncoder[motorC]) < CLAMP && isNotAbort())
    {}
    motor[motorC] = 0;
}

float scanColour(int & colour1, int & colour2)
{
    float dist = 0;
    do{
        displayBigTextLine(3, "Press lower button");
        displayBigTextLine(6, "to scan 1st colour");
        while(!getButtonPress(buttonLeft) && isNotAbort())
        {}
        while(getButtonPress(buttonLeft) && isNotAbort())
        {}
        if(isNotAbort())
        {
            colour1 = SensorValue[S1];
            colour1--;
            if(SensorValue[S1] == (int)colorRed)
                colour1--;
        }
    }
    while((colour1 > 3 || colour1 < 0) && isNotAbort());
    
    wait1Msec(500);
    
    do{
        displayBigTextLine(3, "Press lower button");
        displayBigTextLine(6, "to scan 2nd colour");
        while(!getButtonPress(buttonLeft) && isNotAbort())
        {}
        while(getButtonPress(buttonLeft) && isNotAbort())
        {}
        if(isNotAbort())
        {
            colour2 = SensorValue[S1];
            colour2--;
            if(SensorValue[S1] == (int)colorRed )
                colour2--;
        }
    }
    while((colour2 > 1 || colour2 < 0) && isNotAbort());
    
    eraseDisplay();
    displayString(2, "%d", colour1);
    displayString(4, "%d", colour2);
    wait1Msec(3000);
    dist = storeDist[colour2][colour1];
    wait1Msec(2000);
    eraseDisplay();
    return dist;
}

void getBook(int encoder_lim)
{
    clawClose(SHUT);
    moveFwd(encoder_lim);
    wait1Msec(1000);
    clawOpen(CLAMP);
    wait1Msec(500);
    armExtend();
    wait1Msec(1000);
    clawClose(false);
    armRetract();
    wait1Msec(2000);
    moveBwd();
    wait1Msec(300);
    motor[motorC] = 0;
    clawOpen(CLAMP + 30);
    wait1Msec(500);
    if(!isNotAbort())
    {
        returnPosition();
    }
}

void storeBook(int encoder_lim)
{
    clawOpen(CLAMP);
    while(!SensorValue[S4] && isNotAbort())
    {}
    clawClose(SHUT);
    wait1Msec(1000);
    moveFwd(encoder_lim);
    wait1Msec(1000);
    armExtend();
    wait1Msec(3000);
    clawOpen(CLAMP);
    wait1Msec(1000);
    armRetract();
    wait1Msec(1000);
    clawClose(SHUT);
    wait1Msec(500);
    moveBwd();
    if(!isNotAbort())
    {
        returnPosition();
    }
}

task main()
{
    //declare variables
    //modePlace, where 0 --> off, 1 --> place, 2 --> retrieve
    int modePlace = 0;
    int colour1 = 0, colour2 = 0;
    
    //initializes storage 2D  array
    for(int row = 0; row < 2; row++)
        for(int col = 0; col < 4; col++)
        {
            if(row == 1)
                storage[row][col] = true;
            else
                storage[row][col] = false;
        }
    
    //configure sensors
    SensorType[S4] = sensorEV3_Touch;
    SensorType[S2] = sensorEV3_Touch;
    SensorType[S1] = sensorEV3_Color;
    SensorType[S3] = sensorEV3_Ultrasonic;
    wait1Msec(50);
    SensorMode[S1] = modeEV3Color_Color;
    wait1Msec(50);
    time1[T1] = 0;
    
    //placing and retrieving books
    while(time1[T1] < 300000 && isNotAbort())
    {
        bool isEmpty = true;
        //sets mode
        displayBigTextLine(3, "Select Mode");
        displayBigTextLine(6, "Left:Place");
        displayBigTextLine(9, "Right:Retrieve");
        
        while(!getButtonPress(buttonAny) && isNotAbort())
        {}
        if(getButtonPress(buttonUp))
            modePlace = 1;
        else if(getButtonPress(buttonDown))
            modePlace = 2;
        while((getButtonPress(buttonUp) ||
               getButtonPress(buttonDown)) && isNotAbort())
            
        {}
        eraseDisplay();
        
        //gets colour values
        int dist = scanColour(colour1, colour2);
        if(storage[colour2][colour1] == 1)
            isEmpty = false;
        time1[T1] = 0;
        //Places Books
        if(modePlace == 1 && isEmpty)
        {
            displayString(3, "Placing Book");
            storeBook(dist);
            storage[colour2][colour1] = true;
            displayString(3, "Placed! ");
            wait1Msec(200);
        }
        //Retrieves Books
        else if(modePlace == 2 && !isEmpty)
        {0
            displayString(3, "Retrieving Book");
            getBook(dist);
            storage[colour2][colour1] = false;
            displayString(3, "Retrieved! ");
            wait1Msec(2000);
        }
        else
        {
            displayBigTextLine(3, "Invalid");
        }
        wait1Msec(2000);
    }
    displayString(3, "End");
}
