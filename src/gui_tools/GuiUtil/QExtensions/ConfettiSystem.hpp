#pragma once
#include <QVector>
#include <QPainter>
#include <QRect>


class ConfettiSystem {
    /** Shape enum */
    enum Shape {
        Circle,
        Square
    };

	/** Confetti variables */
    struct Confetti {
        /** The position of the confetti */
        QPointF position;

        /** The velocity of the confetti */
        QPointF velocity;

        /** The size of the confetti */
        qreal size;

		/** The rotation of the confetti */
        qreal rotation;

        /** The rotation speed of the confetti */
        qreal rotationSpeed;

        /** The color speed of the confetti */
        QColor color;

        /** The life span of the confetti */
        qreal lifeSpan;

        /** The shape of the confetti */
        Shape shape;
    };

public:
    /**
     * @brief Constructor
     */
    explicit ConfettiSystem() = default;

    /**
     * @brief Default Destructor
     */
    virtual ~ConfettiSystem() = default;

    void start(const QRect& bounds, int burstCount = 1, int burstConfettiCount = 100, int burstDelayMs = 0);
    void update();
    void spawnConfetti();
    QColor randomColor();
    void draw(QPainter& p);

    bool isActive() const { return _active; }

private:

    static qreal randomRange(qreal min, qreal max);

    /** Variables */
    QRect _boundinRect;
    QVector< Confetti > _pieces;

    int _burstsRemaining = 0;
    int _burstDelayMs = 0;
    qint64 _lastBurstTime = 0;
	int _burstConfettiCount = 100;

    bool _active = false;
};