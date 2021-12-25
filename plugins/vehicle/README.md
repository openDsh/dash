# Vehicle Plugins

## Making a Plugin

Making your own plugin is pretty simple. After doing all of the boiler plate stuff from the Qt docs, the main `dash` elements of the vehicle plugin are the `widgets` and `init` functions.

When loading your plugin into the vehicle page, you will need to perform any initialization steps needed in the `init` function. This is all dependent on your configuration and use case.

If you want to add additional tabs to the vehicle page, you can pass them into `dash` using the `widgets` function. Make sure to set the object name of each widget you add so a proper title can be given for each tab.

## Components

### CANBus Interfacing

canbus/ICANBus.hpp

Any plugin's `init` should take an `ICANBus*` as a parameter.

`ICANBus` provides
`bool writeFrame(QCanBusFrame frame)`

* Writes `frame` to the CANBus, returns true if successful. Note that use of an elm327 only supports sending frames to id `0x7df` (OBD protocol)

`void registerFrameHandler(int id, std::function<void(QByteArray)> callback)`

* Register a callback (`void custom_callback_method(QByteArray data)`) that is called whenever a CANBus message to `id` is detected - where the frame payload is `data`



For instance, if your car sends climate data on CAN id 10, you can register a callback on id 10, and then update dash's climate interface according to the received data.



### Climate Interface
app/widgets/climate.hpp

As a sort of _standard_ component for vehicle plugins, I have included a climate widget in the main project for displaying your vehicle's climate information within dash.

At this time, the widget only acts as a proxy for opening a snackbar (popup window with some info in it). An interface is provided for updating the state of the widget (including the snackbar).

`Climate` contains the following:

```
    Airflow
    {
        OFF
        DEFROST
        BODY
        FEET
    }
```

- `max_fan_speed` - should be set upon construction of the widget

- `fan_speed` - should be between 0 and `max_fan_speed` (inclusive)
- `airflow` - any combination of the `Airflow` enum
- `left_temp` - left side temperature 
- `right_temp` - right side temperature

treat these as _properties_... use `name()` to get the value and `name(value)` to set a new value

Look at the test vehicle plugin for an example of using the `Climate` widget.

### Vehicle Interface
app/widgets/vehicle.hpp

Another _standard_ component for vehicle plugins, the vehicle widget can display various information about your vehicle.

An interface is provided for setting up and updating the state of the widget.

`Vehicle` contains the following:
```
    Position
    {
        NONE
        FRONT
        BACK
        LEFT
        MIDDLE_LEFT
        RIGHT
        MIDDLE_RIGHT
        FRONT_LEFT
        FRONT_RIGHT
        BACK_LEFT
        BACK_RIGHT
        FRONT_MIDDLE_LEFT
        FRONT_MIDDLE_RIGHT
        BACK_MIDDLE_LEFT
        BACK_MIDDLE_RIGHT
    }
```
-  `pressure_init(QString unit, uint8_t threshold)` - enable pressure
-  `disable_sensors()` - prevent sensors from showing (increases size of widget since less space is needed)

-  `rotate(int16_t degree)` - degrees to rotate the entire widget
-  `door(Position position, bool open)` - open/close specific door (FRONT_LEFT, FRONT_RIGHT, BACK_LEFT, BACK_RIGHT)
-  `window(Position position, bool up)` - open/close specific window (FRONT_LEFT, FRONT_RIGHT, BACK_LEFT, BACK_RIGHT)
-  `headlights(bool on)` - toggle headlights
-  `taillights(bool on)` - toggle taillights
-  `indicators(Position position, bool on)` - toggle indicators (LEFT, RIGHT)
-  `hazards(bool on)` - toggle hazards
-  `pressure(Position position, uint8_t value)` - set pressure of specific tire (FRONT_LEFT, FRONT_RIGHT, BACK_LEFT, BACK_RIGHT)
-  `sensor(Position position, uint8_t level)` - set level (0-4) of specific sensor (FRONT_LEFT, FRONT_RIGHT, BACK_LEFT, BACK_RIGHT, FRONT_MIDDLE_LEFT, FRONT_MIDDLE_RIGHT, BACK_MIDDLE_LEFT, BACK_MIDDLE_RIGHT)
-  `wheel_steer(int16_t degree)` - degrees to rotate front tires (dont go too wild here)


Look at the test vehicle plugin for an example of using the `Climate` and `Vehicle` widgets.
