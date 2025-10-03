#ifndef UNIT_HELPERTYPES_H
#define UNIT_HELPERTYPES_H

#include <QDefs>

Q_ENUM_CLASS(RotationDirections, RotationDirection,
              None = 0,
              Clockwise = 1,
              CounterClockwise = 2,
              Both = 3,
              Screwing = 4,
              Unscrewing = 5,
              AntiTrig = 6,
              Trig = 7)

Q_ENUM_CLASS(UnitCategories, UnitCategory,
              InvalidCategory = -1,
              NoCategory = 0,
              Percent = 2,
              Torque = 1000,
              TorquePerTime = 2000,
              AngularVelocity = 3000,
              Angle = 4000,
              Velocity = 5000,
              Time = 6000,
              Mass = 7000,
              Force = 8000,
              Length = 9000)

Q_ENUM_CLASS(UnitTypes, UnitType,
        InvalidUnit = -1,
        NoUnit = 0,
        Neutral = 1,
        Percent = 2,

        // Torque: torqueunit.h
        Torque_mNm = UnitCategories::Torque,
        Torque_cNm,
        Torque_Ncm,
        Torque_Nm,
        Torque_uNm,
        Torque_gmm,
        Torque_gcm,
        Torque_lbfin,
        Torque_ozfin,

        // TorquePerTime: torquepertaimeunit.h
        TorquePerTime_mNm_sec = UnitCategories::TorquePerTime,

        // AngularVelocity: angularvelocityunit.h
        AngularVelocity_tr_min = UnitCategories::AngularVelocity,
        AngularVelocity_deg_min,
        AngularVelocity_deg_s,
        AngularVelocity_tr_s,

        // Angle: angleunit.h
        Angle_deg = UnitCategories::Angle,
        Angle_tr,
        Angle_rad,

        // Velocity: velocityunit.h
        Velocity_m_sec = UnitCategories::Velocity,
        Velocity_km_h,

        // Time: timeunit.h
        Time_msec = UnitCategories::Time,
        Time_sec,
        Time_min,
        Time_hour,

        // Mass: massunit.h
        Mass_gr = UnitCategories::Mass,
        Mass_kg,

        // Force: forceunit.h
        Force_N = UnitCategories::Force,
        Force_mN,
        Force_uN,

        // Length: lengthunit.h
        Length_mm = UnitCategories::Length,
        Length_um,
        Length_m)

class UnitValueType
{
    Q_GADGET
    QML_VALUE_TYPE(unitValueType)
    QML_STRUCTURED_VALUE

    Q_PROPERTY(UnitCategories::Enum category READ getCategory WRITE setCategory FINAL)
    Q_PROPERTY(double value READ getValue WRITE setValue FINAL)
    Q_PROPERTY(UnitTypes::Enum type READ getType WRITE setType FINAL)

public:
    explicit UnitValueType();
    ~UnitValueType() = default;
    UnitValueType(UnitCategory category);
    UnitValueType(UnitCategory category, double value, UnitType type);
    Q_INVOKABLE UnitValueType(const UnitValueType& unitValueType);

    Q_INVOKABLE QString toString() const;

    UnitCategory getCategory() const;
    bool setCategory(UnitCategory category);

    double getValue() const;
    bool setValue(double value);

    UnitType getType() const;
    bool setType(UnitType type);

    UnitValueType& operator=(const UnitValueType &other);
    bool operator==(const UnitValueType& rhs) const { return m_category==rhs.m_category && m_value==rhs.m_value; };
    bool operator< (const UnitValueType& rhs) const { return m_value< rhs.m_value; };
    Q_DECLARE_INSIDE_OPERATORS(UnitValueType)

private:
    UnitCategory m_category;
    double m_value;
    UnitType m_type;
};
Q_DECLARE_METATYPE(UnitValueType)

QDebug operator<<(QDebug dbg, const UnitValueType &unitValueType);

#endif // UNIT_HELPERTYPES_H
