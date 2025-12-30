#include "ConfettiSystem.hpp"

#include <QtMath>
#include <QDateTime>
#include <QRandomGenerator>


void ConfettiSystem::start(const QRect& bounds, const int burstCount, const int burstConfettiCount, const int delayMs)
{
	/** Initialize confetti variables */
    _boundinRect = bounds;
    _pieces.clear();
    _pieces.reserve(_burstConfettiCount * _burstsRemaining + 100);

    _burstConfettiCount = burstConfettiCount;
    _burstsRemaining = burstCount;
    _burstDelayMs = delayMs;
    _lastBurstTime = 0;

    /** Set burst to be active */
    _active = true;
}

void ConfettiSystem::spawnConfetti()
{
    /** Create confetti */
    Confetti confetti;

    /** Set position to the center of the bounding rect */
    confetti.position = QPointF(_boundinRect.width() / 2.0, _boundinRect.height() / 2.0);

    /** Set velocity */
    const qreal angle = randomRange(0, 2 * M_PI);
    const qreal speed = randomRange(2.0, 25.0);
    confetti.velocity = QPointF(std::cos(angle), std::sin(angle)) * speed;

    /** Set size */
    confetti.size = randomRange(8.0, 16.0);

    /** Set rotation and rotation speed */
    confetti.rotation = randomRange(0, 360);
    confetti.rotationSpeed = randomRange(-8, 8);

	/** Set color */
    confetti.color = randomColor();

	/** Set life span */
    confetti.lifeSpan = randomRange(0.9, 1.3);

	/** Set shape */
    confetti.shape = (QRandomGenerator::global()->bounded(2) == 0) ? Circle : Square;

    /** Add confetti to list */
    _pieces.push_back(confetti);
}

QColor ConfettiSystem::randomColor()
{
    static QVector<QColor> colors = {
        QColor("#FACC15"), // gold
        QColor("#F97316"), // orange
        QColor("#EC4899"), // pink
        QColor("#22C55E"), // green
        QColor("#3B82F6"), // blue
        QColor("#A855F7")  // purple
    };

    return colors[QRandomGenerator::global()->bounded(colors.size())];
}

void ConfettiSystem::update()
{
    /** Return if the bursts are no longer active */
    if ( !_active ) {
        return;
    }

    /** Check if it is time to spawn a new burst */
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if ( _burstsRemaining > 0 && ( _lastBurstTime == 0 || now - _lastBurstTime >= _burstDelayMs ) ) {
        /** Spawn burst */
        for ( int i = 0; i < _burstConfettiCount; ++i ) {
            spawnConfetti();
        }

        _burstsRemaining--;
        _lastBurstTime = now;
    }

    /** Loop through pieces and update them */
    int alive = 0;
    for ( auto& c : _pieces ) {
        /** Check if particle life span have been reached and remove it if it has */
        if ( c.lifeSpan <= 0.0 ) {
            continue;
        }

        /** Gravity */
        c.velocity.ry() += 0.15;

        /** Air resistance */
        c.velocity *= 0.99;

        /** Slight horizontal chaos */
        c.velocity.rx() += randomRange(-0.05, 0.05);

		/** Update position and rotation */
        c.position += c.velocity;
        c.rotation += c.rotationSpeed;

        /** Life decay */
        c.lifeSpan -= 0.004;

        /** Check if the particle is still within the bounds */
        if ( c.position.y() < _boundinRect.height() + 100 ) {
            alive++;
        } else {
            /** Set lifespan to 0 */
            c.lifeSpan = 0.0;
		}
    }

    /** Remove dead confetti */
    _pieces.erase(
        std::remove_if(_pieces.begin(), _pieces.end(),
            [](const Confetti& c) {
                return c.lifeSpan <= 0.0;
            }),
        _pieces.end()
    );

	/** Set active state */
    _active = ( alive > 10 ) || ( _burstsRemaining > 0 );
}

void ConfettiSystem::draw(QPainter& painter)
{
    /** Return if the bursts are no longer active */
    if ( !_active ) {
        return;
    }

	/** Save painter state */
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

	/** Loop through pieces and draw them */
    for ( auto& c : _pieces ) {
		/** Check if particle life span have been reached and remove it if it has */
        if ( c.lifeSpan <= 0.0 ) {
            continue;
        }

        painter.save();

		/** Apply transformations */
        painter.translate(c.position);
        painter.rotate(c.rotation);

        /** Draw shape */
        painter.setBrush(c.color);
        painter.setPen(Qt::NoPen);
        if ( c.shape == Square ) {
            painter.drawRect(QRectF(-c.size / 2, -c.size / 2, c.size, c.size * 0.6));
        } else {
            painter.drawEllipse(QPointF(0, 0), c.size / 2, c.size / 2);
        }

		/** Restore painter */
        painter.restore();
    }

    /** Restore painter */
    painter.restore();
}

qreal ConfettiSystem::randomRange(const qreal min, const qreal max)
{
    return min + QRandomGenerator::global()->generateDouble() * (max - min);
}