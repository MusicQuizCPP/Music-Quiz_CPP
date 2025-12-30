#include "ParticleSystem.hpp"

#include <QtMath>
#include <QRandomGenerator>


ParticleSystem::ParticleSystem(int count)
{
    _particles.resize(count);
}

void ParticleSystem::resize(const QRect& rect)
{
    /** Set bounds and respawn particles */
    _boundinRect = rect;
    for ( Particle& particle : _particles ) {
        respawn(particle);
    }
}

void ParticleSystem::respawn(Particle& particle)
{
    /** Create randomizer */
    QRandomGenerator* rng = QRandomGenerator::global();
    auto randomRange = [&](qreal min, qreal max) {
        return min + rng->generateDouble() * ( max - min );
    };

    /** Set position */
    particle.position = QPointF( rng->bounded(_boundinRect.width()), _boundinRect.height() + rng->bounded(100.0));

    /** Set velocity */
    particle.velocity = QPointF(
        randomRange(-0.15, 0.15),   // horizontal drift
        randomRange(-4.0, -0.8)     // upward motion
    );

	/** Set size */
    particle.radius = randomRange(2.0, 5.0);

	/** Set color */
    particle.color = randomColor();
}

void ParticleSystem::update()
{
    /** Update the position of all particles */
    for ( Particle& particle : _particles ) {
        /** Update position based on velocity */
        particle.position += particle.velocity;

        /** Add gentle horizontal wobble */
        particle.position.rx() += qSin(particle.position.y() * 0.02) * 0.1;

		/** Respawn if out of bounds */
        if ( particle.position.y() + particle.radius < 0 ) {
            respawn(particle);
        }
    }
}

QColor ParticleSystem::randomColor()
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

void ParticleSystem::draw(QPainter& painter)
{
	/** Save painter settings */
    painter.save();

    /** Loop through all particles and draw them */
    painter.setPen(Qt::NoPen);
    for ( const Particle& particle : _particles ) {
        painter.setBrush(particle.color);
        painter.drawEllipse(particle.position, particle.radius, particle.radius);
    }

    /** Restore painter settings */
    painter.restore();
}