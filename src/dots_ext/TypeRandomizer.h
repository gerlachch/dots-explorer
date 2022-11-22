#pragma once
#include <random>
#include <dots/type/TypeVisitor.h>

namespace dots::type
{
    struct TypeRandomizerOptions
    {
        bool forceKeyProperties = true;

        inline static timepoint_t MinTimepointDefault = timepoint_t::FromString("1970-01-01T00:00:00+00:00");
        inline static timepoint_t MaxTimepointDefault = timepoint_t::FromString("2038-01-19T04:14:07+00:00");

        timepoint_t minTimepoint = MinTimepointDefault;
        timepoint_t maxTimepoint = MaxTimepointDefault;

        duration_t minDuration =  std::chrono::seconds{ 0 };
        duration_t maxDuration =  std::chrono::seconds{ 157680000 };

        size_t minStringLength = 0;
        size_t maxStringLength = 20;

        size_t minVectorLength = 0;
        size_t maxVectorLength = 20;
    };

    template <typename Engine = std::mt19937_64>
    struct TypeRandomizer : TypeVisitor<TypeRandomizer<Engine>>
    {
        using engine_t = Engine;
        using result_t = typename engine_t::result_type;

        TypeRandomizer(result_t seed = engine_t::default_seed, TypeRandomizerOptions options = {}) :
            m_generator{ seed },
            m_options{ options }
        {
            /* do nothing */
        }
        TypeRandomizer(const TypeRandomizer& other) = default;
        TypeRandomizer(TypeRandomizer&& other) = default;
        ~TypeRandomizer() = default;

        TypeRandomizer& operator = (const TypeRandomizer& rhs) = default;
        TypeRandomizer& operator = (TypeRandomizer&& rhs) = default;

        template <typename T, std::enable_if_t<!std::is_const_v<T> && std::is_base_of_v<Struct, T>, int> = 0>
        T& randomize(T& instance, property_set_t includedProperties)
        {
            visitor_base_t::visit(instance, includedProperties);
            return instance;
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T> && !std::is_base_of_v<Struct, T>, int> = 0>
        T& randomize(T& value, const Descriptor<T>& descriptor)
        {
            visitor_base_t::visit(value, descriptor);
            return value;
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T> && std::is_base_of_v<Struct, T>, int> = 0>
        T& randomize(T& instance)
        {
            property_set_t includedProperties =  randomize<property_set_t>(includedProperties);

            if (m_options.forceKeyProperties)
                includedProperties += instance._keyProperties();

            includedProperties -= instance._validProperties();
            randomize(instance, includedProperties);
            return instance;
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T> && !std::is_base_of_v<Struct, T>, int> = 0>
        T& randomize(T& value)
        {
            visitor_base_t::visit(value);
            return value;
        }

        template <typename T, typename... Args, std::enable_if_t<!std::is_const_v<T> && !std::is_reference_v<T> && std::is_constructible_v<T, Args...>, int> = 0>
        T randomize(Args&&... args)
        {
            T value{ std::forward<Args>(args)... };
            randomize(value);

            return value;
        }

    protected:

        using visitor_base_t = TypeVisitor<TypeRandomizer<Engine>>;

        friend visitor_base_t;

        template <typename T>
        bool visitStructBeginDerived(T&/* instance*/, property_set_t& includedProperties)
        {
            if (includedProperties.empty())
                randomize(includedProperties);

            return true;
        }

        template <typename T>
        bool visitPropertyBeginDerived(T& property, bool/* first*/)
        {
            property.valueOrEmplace();
            return true;
        }

        template <typename T>
        bool visitVectorBeginDerived(vector_t<T>& vector, const Descriptor<vector_t<T>>& descriptor)
        {
            descriptor.fill(vector, std::uniform_int_distribution<size_t>{ m_options.minVectorLength, m_options.maxVectorLength }(m_generator));
            return true;
        }

        template <typename T>
        void visitEnumDerived(T& value, const EnumDescriptor& descriptor)
        {
            const auto& enumerators = descriptor.enumeratorsTypeless();
            descriptor.construct(value, enumerators[std::uniform_int_distribution<size_t>{ 0, enumerators.size() - 1 }(m_generator)].valueTypeless());
        }

        template <typename T>
        void visitFundamentalTypeDerived(T& value, const Descriptor<T>&/* descriptor*/)
        {
            if constexpr(std::is_same_v<T, bool>)
            {
                value = std::uniform_int_distribution<uint16_t>{ 0, 1 }(m_generator);
            }
            else if constexpr(std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>)
            {
                value = static_cast<T>(std::uniform_int_distribution<int16_t>{}(m_generator));
            }
            else if constexpr(std::is_integral_v<T>)
            {
                value = std::uniform_int_distribution<T>{}(m_generator);
            }
            else if constexpr(std::is_floating_point_v<T>)
            {
                value = std::uniform_real_distribution<T>{}(m_generator);
            }
            else if constexpr(std::is_same_v<T, property_set_t>)
            {
                value = property_set_t{ std::uniform_int_distribution<property_set_t::value_t>{}(m_generator) };
            }
            else if constexpr (std::is_same_v<T, timepoint_t>)
            {
                double min = m_options.minTimepoint.duration().toFractionalSeconds();
                double max = m_options.maxTimepoint.duration().toFractionalSeconds();

                value = T{ duration_t{ std::uniform_real_distribution<double>{ min, max }(m_generator) } };
            }
            else if constexpr (std::is_same_v<T, steady_timepoint_t> || std::is_same_v<T, duration_t>)
            {
                double min = m_options.minDuration.toFractionalSeconds();
                double max = m_options.maxDuration.toFractionalSeconds();

                value = T{ duration_t{ std::uniform_real_distribution<double>{ min, max }(m_generator) } };
            }
            else if constexpr (std::is_same_v<T, uuid_t>)
            {
                uuid_t::value_t data;

                for (auto& chunk : reinterpret_cast<uint32_t(&)[4]>(*data.data()))
                {
                    chunk = std::uniform_int_distribution<uint32_t>{}(m_generator);
                }

                value = data;
            }
            else if constexpr (std::is_same_v<T, string_t>)
            {
                constexpr std::string_view Chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

                size_t length = std::uniform_int_distribution<size_t>{ m_options.minStringLength, m_options.maxStringLength }(m_generator);
                value.clear();

                for (size_t i = 0; i < length; ++i)
                {
                    value += Chars[std::uniform_int_distribution<size_t>{ 0, Chars.size() - 1 }(m_generator)];
                }
            }
            else
                static_assert(!std::is_same_v<T, T>, "type not supported");
        }

    private:

        engine_t m_generator;
        TypeRandomizerOptions m_options;
    };
}
