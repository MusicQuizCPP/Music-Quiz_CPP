#pragma once

#include <QRect>
#include <QVector>
#include <QPainter>


class ParticleSystem {
    /** Particle variables */
    struct Particle {
        /** The position of the particle */
        QPointF position;

        /** The velocity of the particle */
        QPointF velocity;

		/** The radius of the particle */
        qreal radius;

		/** The color of the particle */
        QColor color;
    };

    public:
        /**
         * @brief Constructor
         *
         * @param[in] count The number of particles to spawn.
         */
        ParticleSystem(const int count = 200);

        /**
         * @brief Default Destructor
         */
        virtual ~ParticleSystem() = default;

        /**
		 * @brief Resize the particle system bounds.
         * 
		 * @param[in] bounds The new bounding rectangle.
         */
        void resize(const QRect& bounds);

		/**
		 * @brief Updates the position of the particales.
         */
        void update();

		/**
		 * @brief Draws the particles.
         * 
		 * @param[in] painter The painter to draw with.
         */
        void draw(QPainter& painter);

    private:
        /**
		 * @brief Respawns a particle at the bottom with random attributes
         * 
		 * @param[in] particle The particle to respawn
         */
        void respawn(Particle& particle);

		/**
         * @brief Generates a random color for a particle
		 */
        inline QColor randomColor();

        /** Variables */
        QRect _boundinRect;
        QVector< Particle > _particles;
};